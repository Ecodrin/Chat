package utility

import (
	"encoding/json"
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
	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	var config Config
	err = json.NewDecoder(file).Decode(&config)
	if err != nil {
		return nil, err
	}
	return &config, nil
}
