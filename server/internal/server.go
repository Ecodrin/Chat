package server

import (
	"context"
	"database/sql"
	"fmt"
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
	"google.golang.org/grpc/status"
)

type Conns struct {
	data map[string]*pb.Greeter_ChatSessionServer
}

type Server struct {
	pb.UnimplementedGreeterServer

	logger *log.Logger
	config *utility.Config
	DB     *sql.DB

	conns Conns
	mutex sync.RWMutex
}

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
		return nil, status.Error(codes.InvalidArgument, "incorrect login")
	}
	tokenString, err := handlers.GetTokenFromUser(user, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("jwt token generate error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
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
	s.logger.Println("client + " + user.Login + " auth")
	return response, nil
}

func (s *Server) ChatSession(stream pb.Greeter_ChatSessionServer) error {
	var user *handlers.UserHandler
	connect := false
	for {
		select {
		case <-stream.Context().Done():
			if user.ID != "" {
				s.mutex.Lock()
				delete(s.conns.data, user.ID)
				s.mutex.Unlock()
				s.logger.Println("client + " + user.Login + " disconnected")
			}
			return stream.Context().Err()
		default:
			in, err := stream.Recv()
			if err != nil && user != nil {
				if status.Code(err) == codes.Canceled || err == io.EOF {
					s.logger.Println("user " + user.Login + "closed connection")
					s.mutex.Lock()
					delete(s.conns.data, user.ID)
					s.mutex.Unlock()
					return nil
				} else {
					s.logger.Println("stream rcv error: ", err)
					return status.Error(codes.Internal, "")
				}
			} else if err != nil {
				s.logger.Println("stream error: nil user: ", err)
				return nil
			}
			if !connect {
				user, err = handlers.GetUserHandlerFromToken(in.GetToken(), s.config.JWTTokenSecret)
				if err != nil {
					return status.Error(codes.InvalidArgument, "incorrect token")
				}
				s.mutex.Lock()
				s.conns.data[user.ID] = &stream
				fmt.Println(s.conns.data)
				s.mutex.Unlock()
				connect = true
				continue
			}
			// userRecipientStream.SendHeader(metadata.MD{})

			// TODO add kafka
		}
	}

}

func (s *Server) Disconnect(ctx context.Context, req *pb.TokenRequest) (*pb.StatusResponse, error) {
	user, err := handlers.GetUserHandlerFromToken(req.Token, s.config.JWTTokenSecret)
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
	user, err := handlers.GetUserHandlerFromToken(req.Token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}
	userContact, err := database.GetUserByLogin(s.DB, req.Contact)
	if err != nil {
		s.logger.Println("database get user by login error:", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	s.mutex.Lock()
	contactStream, ok := s.conns.data[userContact.ID]
	fmt.Println(s.conns.data)
	s.mutex.Unlock()
	if !ok {
		return nil, status.Error(codes.InvalidArgument, "incorrect contact or contact not online")
	}
	err = (*contactStream).Send(&pb.OutpuMsg{
		Data:      user.Login,
		StatusMsg: 1, // заявка на добавление в друзья
		Id:        uuid.NewString(),
	})
	response := &pb.StatusResponse{
		Status: 0,
	}
	return response, nil
}

func (s *Server) AcceptRequestContact(ctx context.Context, req *pb.NewContactRequest) (*pb.StatusResponse, error) {
	user, err := handlers.GetUserHandlerFromToken(req.Token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}
	err = database.AddContactByID(s.DB, user.ID, req.Contact)
	if err != nil {
		s.logger.Println("database add contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	userContact, err := database.GetUserByLogin(s.DB, req.Contact)
	if err != nil {
		s.logger.Println("database get user by login error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	err = database.AddContactByID(s.DB, userContact.ID, user.Login)
	if err != nil {
		s.logger.Println("database add contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	response := &pb.StatusResponse{
		Status: 0,
	}
	return response, nil
}

func (s *Server) GetContacts(ctx context.Context, req *pb.TokenRequest) (*pb.GetContactsResponse, error) {
	user, err := handlers.GetUserHandlerFromToken(req.Token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}

	contacts, err := database.GetLoginContactsByID(s.DB, user.ID)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "noone contact")
	}

	response := &pb.GetContactsResponse{
		Contats: contacts,
	}
	return response, nil
}

func (s *Server) DeleteContact(ctx context.Context, req *pb.DeleteContactRequest) (*pb.StatusResponse, error) {
	user, err := handlers.GetUserHandlerFromToken(req.Token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect token")
	}
	contactUser, err := database.GetUserByLogin(s.DB, req.Contact)
	if err != nil {
		s.logger.Println("database get user by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}
	err = database.DeleteContactByID(s.DB, user.ID, req.Contact)
	if err != nil {
		s.logger.Println("database delete contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}

	err = database.DeleteContactByID(s.DB, contactUser.ID, user.Login)
	if err != nil {
		s.logger.Println("database delete contact by id error: ", err)
		return nil, status.Error(codes.InvalidArgument, "incorrect contact")
	}

	response := &pb.StatusResponse{
		Status: 0,
	}
	return response, nil
}

func StartServer() {
	config, err := utility.LoadConfig("configs/config.json")
	if err != nil {
		panic(err)
	}

	log_file, err := os.Create(config.LogsDirectory + "/" + "server.log")
	if err != nil {
		panic(err)
	}
	defer log_file.Close()
	logger := *log.New(log_file, "", log.LstdFlags)

	lis, err := net.Listen("tcp", config.Address+":"+config.Port)
	if err != nil {
		panic(err)
	}

	DB, err := database.InitDB(config.DataBaseUser + ":" + config.DataBasePassword +
		"@tcp(" + config.DataBaseAddress + ":" + config.DataBasePort + ")/" + config.DataBaseName + "?charset=utf8mb4&parseTime=True&loc=Local")
	if err != nil {
		logger.Fatal("database error init: ", err)
		panic(err)
	}

	err = database.RunMigrations(DB)
	if err != nil {
		logger.Fatal("database error migration: ", err)
		panic(err)
	}

	s := grpc.NewServer()
	pb.RegisterGreeterServer(s, &Server{
		logger: &logger,
		config: config,
		DB:     DB,
		conns:  Conns{data: make(map[string]*pb.Greeter_ChatSessionServer)},
	})

	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, syscall.SIGTERM, syscall.SIGINT)

	go func() {
		if err := s.Serve(lis); err != nil {
			logger.Fatal(err)
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

	select {
	case <-done:
	case <-ctx.Done():
		s.Stop()
	}

	logger.Println("Server stopped")
}
