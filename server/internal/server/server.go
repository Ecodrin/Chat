package server

import (
	"context"
	"database/sql"
	"io"
	"log"
	"net"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"

	"server/internal/database"
	pb "server/internal/grps_chat"

	"server/internal/handlers"
	utility "server/internal/utility"

	"github.com/google/uuid"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/metadata"
	"google.golang.org/grpc/status"
	"google.golang.org/protobuf/encoding/protojson"

	amqp "github.com/rabbitmq/amqp091-go"
)

type Conns struct {
	data map[string]*pb.Greeter_ChatSessionServer
}

type Server struct {
	pb.UnimplementedGreeterServer

	logger          *log.Logger
	config          *utility.Config
	DB              *sql.DB
	queueMQRabbit   amqp.Queue
	channelMQRabbit *amqp.Channel

	conns Conns
	mutex sync.RWMutex
}

var (
	tokenMetadataID = "authorization"
)

func (s *Server) Registration(ctx context.Context, req *pb.RegistrationRequest) (*pb.StatusRegistrationAuthResponse, error) {
	user := handlers.UserHandler{
		Login:    req.Login,
		Password: req.Password,
		ID:       uuid.NewString(),
		Online:   true,
	}

	err := database.CreateUser(s.DB, user)
	if err != nil {
		s.logger.Println("database create user error: ", err)
		return nil, status.Error(codes.InvalidArgument, "none exist user")
	}
	tokenString, err := handlers.GetTokenFromUser(user, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("jwt token generate error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect data")
	}
	response := &pb.StatusRegistrationAuthResponse{
		Status: 0,
		Token:  tokenString,
	}
	s.logger.Println("client " + user.Login + " register")
	return response, nil
}

func (s *Server) Auth(ctx context.Context, req *pb.AuthRequest) (*pb.StatusRegistrationAuthResponse, error) {
	user, err := database.GetUserByLogin(s.DB, req.Login)
	if err != nil {
		if err == sql.ErrNoRows {
			return nil, status.Error(codes.InvalidArgument, "incorrect login")
		}
		s.logger.Println("database get user by login error: ", err)
		return &pb.StatusRegistrationAuthResponse{Status: 2}, err
	}

	if user.Online {
		return nil, status.Error(codes.AlreadyExists, "user already online")
	}

	err = database.UpdateUserOnline(s.DB, user.ID, true)
	if err != nil {
		s.logger.Println("database update user online error: ", err)
		return nil, status.Error(codes.Internal, "")
	}

	tokenString, err := handlers.GetTokenFromUser(*user, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("jwt token generate error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect data")
	}
	response := &pb.StatusRegistrationAuthResponse{
		Status: 0,
		Token:  tokenString,
	}
	s.logger.Println("client " + user.Login + " auth")
	return response, nil
}

func (s *Server) ChatSession(stream pb.Greeter_ChatSessionServer) error {
	var user *handlers.UserHandler
	md, ok := metadata.FromIncomingContext(stream.Context())
	if !ok {
		return status.Error(codes.InvalidArgument, "missing metadata")
	}
	tokenVector := md[tokenMetadataID]
	if len(tokenVector) == 0 {
		return status.Error(codes.InvalidArgument, "no token in metadata")
	}
	token := tokenVector[0]
	user, err := handlers.GetUserHandlerFromToken(token, s.config.JWTTokenSecret)
	if err != nil {
		return status.Error(codes.InvalidArgument, "incorrect token")
	}
	s.mutex.Lock()
	s.conns.data[user.ID] = &stream
	s.mutex.Unlock()
	for {
		select {
		case <-stream.Context().Done():
			if user.ID != "" {
				s.mutex.Lock()
				delete(s.conns.data, user.ID)
				s.mutex.Unlock()
				err := database.UpdateUserOnline(s.DB, user.ID, false)
				if err != nil {
					s.logger.Println("update user online error: ", err)
				}
				s.logger.Println("client + " + user.Login + " disconnected")
			}
			return stream.Context().Err()
		default:
			msg, err := stream.Recv()
			if err != nil && user != nil {
				if status.Code(err) == codes.Canceled || err == io.EOF {
					s.logger.Println("user " + user.Login + " closed connection")
					s.mutex.Lock()
					delete(s.conns.data, user.ID)
					s.mutex.Unlock()
					err := database.UpdateUserOnline(s.DB, user.ID, false)
					if err != nil {
						s.logger.Println("update user online error: ", err)
					}
					return nil
				} else {
					s.logger.Println("stream rcv error: ", err)
					return status.Error(codes.Internal, "")
				}
			} else if err != nil {
				s.logger.Println("stream error: nil user: ", err)
				return nil
			}

			marshaler := protojson.MarshalOptions{
				EmitUnpopulated: true,
				UseProtoNames:   true,
			}

			JSONmsg, err := marshaler.Marshal(msg)
			if err != nil {
				s.logger.Println("json marshal error: ", err)
				continue
			}

			err = s.channelMQRabbit.Publish(
				"",
				s.queueMQRabbit.Name,
				false,
				false,
				amqp.Publishing{
					ContentType: "application/json",
					Body:        JSONmsg,
				},
			)
		}
	}

}

func (s *Server) Disconnect(ctx context.Context, req *pb.EmptyMsg) (*pb.StatusResponse, error) {
	md, ok := metadata.FromIncomingContext(ctx)
	if !ok {
		return nil, status.Error(codes.InvalidArgument, "missing metadata")
	}
	tokenVector := md[tokenMetadataID]
	if len(tokenVector) == 0 {
		return nil, status.Error(codes.InvalidArgument, "no token in metadata")
	}
	token := tokenVector[0]
	user, err := handlers.GetUserHandlerFromToken(token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}
	err = database.UpdateUserOnline(s.DB, user.ID, false)
	if err != nil {
		s.logger.Println("database update user online error: ", err)
		return nil, status.Error(codes.Internal, "")
	}
	response := &pb.StatusResponse{
		Status: 0,
	}
	s.logger.Println("client " + user.Login + " disconnect")
	return response, nil
}

func (s *Server) AddContact(ctx context.Context, req *pb.NewContactRequest) (*pb.StatusResponse, error) {
	md, ok := metadata.FromIncomingContext(ctx)
	if !ok {
		return nil, status.Error(codes.InvalidArgument, "missing metadata")
	}
	tokenVector := md[tokenMetadataID]
	if len(tokenVector) == 0 {
		return nil, status.Error(codes.InvalidArgument, "no token in metadata")
	}
	token := tokenVector[0]
	user, err := handlers.GetUserHandlerFromToken(token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}
	userContact, err := database.GetUserByLogin(s.DB, req.Contact)
	if err != nil {
		s.logger.Println("database get user by login error:", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	if !ok {
		return nil, status.Error(codes.InvalidArgument, "incorrect contact or contact not online")
	}

	err = database.AddContactByID(s.DB, user.ID, req.Contact, 1)
	if err != nil {
		s.logger.Println("database add contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}

	err = database.AddContactByID(s.DB, userContact.ID, user.Login, 2)
	if err != nil {
		s.logger.Println("database add contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	response := &pb.StatusResponse{
		Status: 0,
	}
	s.SendToMQRabbit(&pb.ChatMsg{
		Payload: &pb.ChatMsg_ContactNotification{
			ContactNotification: &pb.ContactMsgNotification{
				Contact: req.Contact,
				Sender:  user.Login,
			},
		},
	})
	return response, nil
}

func (s *Server) AcceptRequestContact(ctx context.Context, req *pb.NewContactRequest) (*pb.StatusResponse, error) {
	md, ok := metadata.FromIncomingContext(ctx)
	if !ok {
		return nil, status.Error(codes.InvalidArgument, "missing metadata")
	}
	tokenVector := md[tokenMetadataID]
	if len(tokenVector) == 0 {
		return nil, status.Error(codes.InvalidArgument, "no token in metadata")
	}
	token := tokenVector[0]
	user, err := handlers.GetUserHandlerFromToken(token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}
	userContact, err := database.GetUserByLogin(s.DB, req.Contact)
	if err != nil {
		s.logger.Println("database get user by login error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	err = database.UpdateStatusContactByID(s.DB, userContact.ID, user.Login, 0)
	if err != nil {
		s.logger.Println("database update contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}

	err = database.UpdateStatusContactByID(s.DB, user.ID, userContact.Login, 0)
	if err != nil {
		s.logger.Println("database update contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	response := &pb.StatusResponse{
		Status: 0,
	}
	s.SendToMQRabbit(&pb.ChatMsg{
		Payload: &pb.ChatMsg_ContactNotification{
			ContactNotification: &pb.ContactMsgNotification{
				Contact: req.Contact,
				Sender:  user.Login,
			},
		},
	})

	return response, nil
}

func (s *Server) DeclineRequestContact(ctx context.Context, req *pb.NewContactRequest) (*pb.StatusResponse, error) {
	md, ok := metadata.FromIncomingContext(ctx)
	if !ok {
		return nil, status.Error(codes.InvalidArgument, "missing metadata")
	}
	tokenVector := md[tokenMetadataID]
	if len(tokenVector) == 0 {
		return nil, status.Error(codes.InvalidArgument, "no token in metadata")
	}
	token := tokenVector[0]
	user, err := handlers.GetUserHandlerFromToken(token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}
	userContact, err := database.GetUserByLogin(s.DB, req.Contact)
	if err != nil {
		s.logger.Println("database get user by login error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	err = database.DeleteContactByID(s.DB, userContact.ID, user.Login)
	if err != nil {
		s.logger.Println("database add contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}

	err = database.DeleteContactByID(s.DB, user.ID, userContact.Login)
	if err != nil {
		s.logger.Println("database add contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}

	response := &pb.StatusResponse{
		Status: 0,
	}
	s.SendToMQRabbit(&pb.ChatMsg{
		Payload: &pb.ChatMsg_ContactNotification{
			ContactNotification: &pb.ContactMsgNotification{
				Contact: req.Contact,
				Sender:  user.Login,
			},
		},
	})
	return response, nil
}

func (s *Server) GetContacts(ctx context.Context, req *pb.EmptyMsg) (*pb.GetContactsResponse, error) {
	md, ok := metadata.FromIncomingContext(ctx)
	if !ok {
		return nil, status.Error(codes.InvalidArgument, "missing metadata")
	}
	tokenVector := md[tokenMetadataID]
	if len(tokenVector) == 0 {
		return nil, status.Error(codes.InvalidArgument, "no token in metadata")
	}
	token := tokenVector[0]
	user, err := handlers.GetUserHandlerFromToken(token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}

	contacts, statuses, err := database.GetContactsByID(s.DB, user.ID)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "noone contact")
	}

	response := &pb.GetContactsResponse{
		Contats:  contacts,
		Statuses: statuses,
	}

	return response, nil
}

func (s *Server) DeleteContact(ctx context.Context, req *pb.DeleteContactRequest) (*pb.StatusResponse, error) {
	md, ok := metadata.FromIncomingContext(ctx)
	if !ok {
		return nil, status.Error(codes.InvalidArgument, "missing metadata")
	}
	tokenVector := md[tokenMetadataID]
	if len(tokenVector) == 0 {
		return nil, status.Error(codes.InvalidArgument, "no token in metadata")
	}
	token := tokenVector[0]
	user, err := handlers.GetUserHandlerFromToken(token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}
	_, err = database.GetUserByLogin(s.DB, req.Contact)
	if err != nil {
		s.logger.Println("database get user by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	err = database.DeleteContactByID(s.DB, user.ID, req.Contact)
	if err != nil {
		s.logger.Println("database delete contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}

	userContact, err := database.GetUserByLogin(s.DB, req.Contact)
	if err != nil {
		s.logger.Println("database get user by login error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	err = database.DeleteContactByID(s.DB, userContact.ID, user.Login)
	if err != nil {
		s.logger.Println("database delete contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}

	response := &pb.StatusResponse{
		Status: 0,
	}
	s.SendToMQRabbit(&pb.ChatMsg{
		Payload: &pb.ChatMsg_ContactNotification{
			ContactNotification: &pb.ContactMsgNotification{
				Contact: req.Contact,
				Sender:  user.Login,
			},
		},
	})
	return response, nil
}

func StartServer() {
	config, err := utility.LoadConfig("configs/config.json")
	if err != nil {
		panic(err)
	}

	// log_file, err := os.Create(config.LogsDirectory + "/" + "server.log")
	// if err != nil {
	// 	panic(err)
	// }
	// defer log_file.Close()
	// logger := *log.New(log_file, "", log.LstdFlags)
	logger := *log.Default()
	logger.Println("logger init successful")

	lis, err := net.Listen("tcp", config.Address+":"+config.Port)
	if err != nil {
		panic(err)
	}
	logger.Println("open tcp connection successful")

	DB, err := database.InitDB(config.DataBaseURLCoonntection)
	if err != nil {
		logger.Fatal("database error init: ", err)
		panic(err)
	}
	logger.Println("init DB successful")

	err = database.RunMigrations(DB)
	if err != nil {
		logger.Fatal("database error migration: ", err)
		panic(err)
	}

	connMQRabbit, err := amqp.Dial(config.MQRabbitURLConnection)
	if err != nil {
		logger.Fatalln("amqp Dial error: ", err)
		return
	}
	channelMQRabbit, err := connMQRabbit.Channel()
	if err != nil {
		logger.Fatalln("connMqRabbit Channel error: ", err)
		return
	}
	queueMQRabbit, err := channelMQRabbit.QueueDeclare(
		config.MQRabbitMsgQueueName,
		true,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		logger.Fatalln("Queue Declare error: ", err)
		return
	}

	logger.Println("init mq rabbit successful")

	s := grpc.NewServer()
	server := &Server{
		logger:          &logger,
		config:          config,
		DB:              DB,
		queueMQRabbit:   queueMQRabbit,
		channelMQRabbit: channelMQRabbit,
		conns:           Conns{data: make(map[string]*pb.Greeter_ChatSessionServer)},
	}

	pb.RegisterGreeterServer(s, server)

	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, syscall.SIGTERM, syscall.SIGINT)

	go func() {
		if err := s.Serve(lis); err != nil {
			logger.Fatal(err)
			panic(err)
		}
	}()

	go func() {
		if err := server.MsgsProccessing(); err != nil {
			logger.Fatalln(err)
			panic(err)
		}
	}()

	logger.Println("server start at :" + config.Port)

	<-sigs
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	done := make(chan bool, 1)
	go func() {
		s.GracefulStop()
		logger.Println("gRPC-server stopped")
		done <- true
	}()

	_, err = server.DB.Exec("update users set online = 0;")
	if err != nil {
		server.logger.Println("update users set online = 0 error: ", err)
	}

	select {
	case <-done:
	case <-ctx.Done():
		s.Stop()
	}

	logger.Println("Server stopped")
}
