package server

import (
	"encoding/json"
	"os"
)

type Config struct {
	Address string `json:"address"`
	Port    string `json:"port"`
}

func loadConfig(filename string) (*Config, error) {
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
