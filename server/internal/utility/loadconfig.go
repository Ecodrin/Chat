package utility

import (
	"os"
)

type Config struct {
	Address       string `json:"address"`
	Port          string `json:"port"`
	LogsDirectory string `json:"logs directory"`

	DataBaseURLCoonntection string `json:"database_url_connection"`

	JWTTokenSecret        string `json:"jwt token secret"`
	MQRabbitURLConnection string `json:"mqrabbit_url_connection"`
	MQRabbitMsgQueueName  string `json:"mqrabbit_msgs_queue_name"`
}

func LoadConfig(filename string) (*Config, error) {
	// file, err := os.Open(filename)
	// if err != nil {
	// 	return nil, err
	// }
	// var config Config
	// err = json.NewDecoder(file).Decode(&config)
	// if err != nil {
	// 	return nil, err
	// }

	config := Config{
		Address:                 os.Getenv("HOST"),
		Port:                    os.Getenv("PORT"),
		LogsDirectory:           "",
		DataBaseURLCoonntection: os.Getenv("DataBaseURLCoonntection"),
		JWTTokenSecret:          os.Getenv("JWTTokenSecret"),
		MQRabbitURLConnection:   os.Getenv("MQRabbitURLConnection"),
		MQRabbitMsgQueueName:    os.Getenv("MQRabbitMsgQueueName"),
	}
	return &config, nil
}
