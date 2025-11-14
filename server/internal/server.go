package server

import (
	"context"
	"database/sql"
	"fmt"
	"log"
	"net"
	"os"
	"os/signal"
	"syscall"
	"time"

	"server/internal/database"
	pb "server/internal/grps_chat"

	"server/internal/handlers"
	utility "server/internal/utility"

	"github.com/google/uuid"
	"google.golang.org/grpc"
)

type Server struct {
	pb.UnimplementedGreeterServer

	logger *log.Logger
	config *utility.Config
	DB     *sql.DB
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
		response := &pb.StatusRegistrationAuthResponse{
			Status: 1,
		}
		return response, fmt.Errorf("incorrect login")
	}
	tokenString, err := handlers.GetTokenFromUser(user, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("jwt token generate error: ", err)
		response := &pb.StatusRegistrationAuthResponse{
			Status: 0,
		}
		return response, fmt.Errorf("incorrect data")
	}
	response := &pb.StatusRegistrationAuthResponse{
		Status: 0,
		Token:  tokenString,
	}
	return response, nil
}

func (s *Server) Auth(ctx context.Context, req *pb.AuthRequest) (*pb.StatusRegistrationAuthResponse, error) {
	user, err := database.GetUserByLogin(s.DB, req.Login)
	if err != nil {
		if err == sql.ErrNoRows {
			response := &pb.StatusRegistrationAuthResponse{
				Status: 1,
			}
			return response, fmt.Errorf("incorrect login")
		}
		s.logger.Println("database get user by login error: ", err)
		return &pb.StatusRegistrationAuthResponse{Status: 2}, err
	}

	if user.Online {
		response := &pb.StatusRegistrationAuthResponse{
			Status: 1,
		}
		return response, fmt.Errorf("user already online")
	}

	err = database.UpdateUserOnline(s.DB, user.ID, true)
	if err != nil {
		s.logger.Println("database update user online error: ", err)
		response := &pb.StatusRegistrationAuthResponse{
			Status: 2,
		}
		return response, fmt.Errorf("internal error")
	}

	tokenString, err := handlers.GetTokenFromUser(*user, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("jwt token generate error: ", err)
		response := &pb.StatusRegistrationAuthResponse{
			Status: 1,
		}
		return response, fmt.Errorf("incorrect data")
	}
	response := &pb.StatusRegistrationAuthResponse{
		Status: 0,
		Token:  tokenString,
	}
	return response, nil
}

func (s *Server) ChatSession(stream pb.Greeter_ChatSessionServer) error {
	// TOD
	return nil
}

func (s *Server) Disconnect(ctx context.Context, req *pb.TokenRequest) (*pb.StatusResponse, error) {
	user, err := handlers.GetUserHandlerFromToken(req.Token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		response := &pb.StatusResponse{
			Status: 1,
		}
		return response, fmt.Errorf("incorrect token")
	}
	err = database.UpdateUserOnline(s.DB, user.ID, false)
	if err != nil {
		s.logger.Println("database update user online error: ", err)
		response := &pb.StatusResponse{
			Status: 2,
		}
		return response, fmt.Errorf("internal error")
	}
	response := &pb.StatusResponse{
		Status: 0,
	}
	return response, nil
}

func (s *Server) AddContact(ctx context.Context, req *pb.NewContactRequest) (*pb.StatusResponse, error) {
	user, err := handlers.GetUserHandlerFromToken(req.Token, s.config.JWTTokenSecret)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		response := &pb.StatusResponse{
			Status: 1,
		}
		return response, fmt.Errorf("incorrect token")
	}
	// TODO отправка контакту запроса в друзья
	err = database.AddContactByID(s.DB, user.ID, req.Contact)
	if err != nil {
		s.logger.Println("database add contact by id error: ", err)
		response := &pb.StatusResponse{
			Status: 1,
		}
		return response, fmt.Errorf("incorrect contact")
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
		return nil, fmt.Errorf("incorrect token")
	}

	contacts, err := database.GetLoginContactsByID(s.DB, user.ID)
	if err != nil {
		s.logger.Println("get user handler from token error: ", err)
		return nil, fmt.Errorf("noone contact")
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
		response := &pb.StatusResponse{
			Status: 1,
		}
		return response, fmt.Errorf("incorrect token")
	}
	// TODO отправка контакту об удалению из друзей
	err = database.DeleteContactByID(s.DB, user.ID, req.Contact)
	if err != nil {
		s.logger.Println("database delete contact by id error: ", err)
		response := &pb.StatusResponse{
			Status: 1,
		}
		return response, fmt.Errorf("incorrect contact")
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
