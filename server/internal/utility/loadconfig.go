package utility

import (
	"encoding/json"
	"os"
)

type Config struct {
	Address       string `json:"address"`
	Port          string `json:"port"`
	LogsDirectory string `json:"logs directory"`
	KafkaAddress  string `json:"kafka address"`
	KafkaPort     string `json:"kafka port"`

	DataBaseUser     string `json:"database user"`
	DataBaseName     string `json:"database name"`
	DataBasePassword string `json:"database password"`
	DataBaseAddress  string `json:"database address"`
	DataBasePort     string `json:"database port"`

	JWTTokenSecret string `json:"jwt token secret"`
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
