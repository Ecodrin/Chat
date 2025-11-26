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
		var msgH pb.InputMsg
		err = json.Unmarshal(msg.Body, &msgH)
		if err != nil {
			s.logger.Println("json unmarshal in MsgsProccessing error: ", err)
			continue
		}

		_, err := database.GetUserByLogin(s.DB, msgH.Sender)
		if err != nil {
			s.logger.Println("no real user error: ", err)
			continue
		}

		userRecipient, err := database.GetUserByLogin(s.DB, msgH.Recipient)
		if err != nil {
			s.logger.Println("no real user error: ", err)
			continue
		}

		s.mutex.Lock()
		recipientStream, ok := s.conns.data[userRecipient.ID]
		s.mutex.Unlock()
		if !ok {
			s.logger.Println("none exist user "+msgH.Recipient+" in MsgsProccessing error: ", err)
			continue
		}
		outputMsg := &pb.OutputMsg{
			Sender:         msgH.Sender,
			Recipient:      msgH.Recipient,
			Data:           msgH.Data,
			Id:             msgH.Id,
			Timestamp:      msgH.Timestamp,
			StatusMsg:      msgH.StatusMsg,
			IsFile:         msgH.IsFile,
			IndexFileChunk: msgH.IndexFileChunk,
			TotalFileChunk: msgH.TotalFileChunk,
		}
		err = (*recipientStream).Send(outputMsg)
		if err != nil {
			s.logger.Println("send msg in MsgsProccessing error: ", err)
			continue
		}
		// DEBUG
		s.logger.Println("msg " + string(msgH.Data) + "proccessing")
	}
	return nil
}
