package server

import (
	"encoding/json"
	"server/internal/database"
	pb "server/internal/grps_chat"
)

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

	for msg := range msgs {
		var msgH pb.ChatMsg
		err = json.Unmarshal(msg.Body, &msgH)
		if err != nil {
			s.logger.Println("json unmarshal in MsgsProccessing error: ", err)
			continue
		}

		var sender, recipient string
		switch msgH.Payload.(type) {
		case *pb.ChatMsg_NewChatMsg:
			sender = msgH.GetNewChatMsg().Sender
			recipient = msgH.GetNewChatMsg().GetRecipient()
		case *pb.ChatMsg_FileMsg:
			sender = msgH.GetFileMsg().Sender
			recipient = msgH.GetFileMsg().Recipient
		case *pb.ChatMsg_DefaultMsg:
			sender = msgH.GetDefaultMsg().Sender
			recipient = msgH.GetDefaultMsg().GetRecipient()
		default:
			s.logger.Println("unrecognized type of msg")
			continue
		}

		_, err := database.GetUserByLogin(s.DB, sender)
		if err != nil {
			s.logger.Println("no real user error: ", err)
			continue
		}

		userRecipient, err := database.GetUserByLogin(s.DB, recipient)
		if err != nil {
			s.logger.Println("no real user error: ", err)
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
		err = (*recipientStream).Send(outputMsg)
		if err != nil {
			s.logger.Println("send msg in MsgsProccessing error: ", err)
			continue
		}
	}
	return nil
}
