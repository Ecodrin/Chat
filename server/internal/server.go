package server

import (
	"context"
	"log"
	"net"

	pb "server/internal/grps_chat"

	"google.golang.org/grpc"
)

type Server struct {
	pb.UnimplementedGreeterServer
}

func (s *Server) Registration(ctx context.Context, req *pb.RegistrationRequest) (*pb.StatusResponse, error) {
	// TODO check validation
	response := &pb.StatusResponse{
		Status: 0,
	}
	return response, nil
}

func (s *Server) Auth(ctx context.Context, req *pb.AuthRequest) (*pb.StatusResponse, error) {
	// TODO check exist
	response := &pb.StatusResponse{
		Status: 0,
	}
	return response, nil
}

func (s *Server) ChatSession(stream pb.Greeter_ChatSessionServer) error {

	return nil
}

func StartServer() {
	config, err := loadConfig("../../configs/config.json")
	if err != nil {
		panic(err)
	}
	lis, err := net.Listen("tcp", config.Address+":"+config.Port)
	if err != nil {
		panic(err)
	}
	log.Println("server start at :" + config.Port)

	s := grpc.NewServer()
	pb.RegisterGreeterServer(s, &Server{})

	if err := s.Serve(lis); err != nil {
		log.Fatalf("Ошибка при запуске сервера: %v", err)
	}
}
