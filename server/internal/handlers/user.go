package handlers

import (
	"fmt"
	"time"

	"github.com/golang-jwt/jwt/v5"
)

type UserHandler struct {
	ID       string `json:"id"`
	Login    string `json:"login"`
	Password string `json:"password"`
	Online   bool   `json:"online"`
}

type Claims struct {
	Exp  int
	User UserHandler `json:"user"`
	jwt.RegisteredClaims
}

func GetUserHandlerFromToken(token string, secretToken string) (*UserHandler, error) {
	claims := &Claims{}
	outputToken, err := jwt.ParseWithClaims(token, claims, func(token *jwt.Token) (interface{}, error) {
		if _, ok := token.Method.(*jwt.SigningMethodHMAC); !ok {
			return nil, fmt.Errorf("bad sign method")
		}
		return []byte(secretToken), nil
	})
	if err != nil {
		return nil, err
	}
	if !outputToken.Valid {
		return nil, fmt.Errorf("incorrect token")
	}
	return &claims.User, nil
}

func GetTokenFromUser(user UserHandler, secretToken string) (string, error) {
	claims := &Claims{
		Exp:              int(time.Now().Add(24 * time.Hour).Unix()),
		User:             user,
		RegisteredClaims: jwt.RegisteredClaims{},
	}

	token := jwt.NewWithClaims(jwt.SigningMethodHS256, claims)
	tokenString, err := token.SignedString([]byte(secretToken))
	if err != nil {
		return "", err
	}
	return tokenString, nil
}
