package server

import (
	"server/internal/database"
	pb "server/internal/grps_chat"
	"time"

	amqp "github.com/rabbitmq/amqp091-go"
	"google.golang.org/protobuf/encoding/protojson"
)

func (s *Server) SendToMQRabbit(msg *pb.ChatMsg) {
	marshaler := protojson.MarshalOptions{
		EmitUnpopulated: true,
		UseProtoNames:   true,
	}

	JSONmsg, err := marshaler.Marshal(msg)
	if err != nil {
		s.logger.Println("json marshal error: ", err)
		return
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

func (s *Server) MsgsProccessing() error {
	msgs, err := s.channelMQRabbit.Consume(
		s.config.MQRabbitMsgQueueName,
		"",
		true,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		return err
	}

	unmarshaler := protojson.UnmarshalOptions{
		DiscardUnknown: true,
	}
	for msg := range msgs {
		var msgH pb.ChatMsg
		err = unmarshaler.Unmarshal(msg.Body, &msgH)
		if err != nil {
			s.logger.Println("json unmarshal in MsgsProccessing error: ", err)
			continue
		}

		var sender, recipient string
		switch msgH.Payload.(type) {
		case *pb.ChatMsg_NewChatMsg:
			sender = msgH.GetNewChatMsg().GetSender()
			recipient = msgH.GetNewChatMsg().GetRecipient()
		case *pb.ChatMsg_FileMsg:
			sender = msgH.GetFileMsg().GetSender()
			recipient = msgH.GetFileMsg().GetRecipient()
		case *pb.ChatMsg_DefaultMsg:
			sender = msgH.GetDefaultMsg().GetSender()
			recipient = msgH.GetDefaultMsg().GetRecipient()
		case *pb.ChatMsg_DeleteChatMsg:
			sender = msgH.GetDeleteChatMsg().GetSender()
			recipient = msgH.GetDeleteChatMsg().GetRecipient()
		case *pb.ChatMsg_ContactNotification:
			recipient = msgH.GetContactNotification().GetContact()
			sender = msgH.GetContactNotification().GetSender()
		default:
			s.logger.Println("unrecognized type of msg")
			continue
		}

		_, err := database.GetUserByLogin(s.DB, sender)
		if err != nil {
			s.logger.Println("no real user sender "+sender+" in MsgsProccessing error: ", err)
			continue
		}

		userRecipient, err := database.GetUserByLogin(s.DB, recipient)
		if err != nil {
			s.logger.Println("no real user recipient "+recipient+" in MsgsProccessing error: ", err)
			continue
		}

		s.mutex.Lock()
		recipientStream, ok := s.conns.data[userRecipient.ID]
		s.mutex.Unlock()
		if !ok {
			s.logger.Println("not online user "+recipient+" in MsgsProccessing error: ", err)
			continue
		}
		outputMsg := &msgH
		s.logger.Println("msg send at", time.Now(), "to ", recipient)
		err = (*recipientStream).Send(outputMsg)
		if err != nil {
			s.logger.Println("send msg in MsgsProccessing error: ", err)
			continue
		}
	}
	return nil
}
