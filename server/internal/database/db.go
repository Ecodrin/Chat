package database

import (
	"database/sql"
	"time"

	_ "github.com/go-sql-driver/mysql"

	"server/internal/handlers"
)

func InitDB(dataBaseName string) (*sql.DB, error) {
	DB, err := sql.Open("mysql", dataBaseName)
	if err != nil {
		return nil, err
	}
	err = DB.Ping()
	if err != nil {
		return nil, err
	}
	return DB, nil
}

func CreateUser(DB *sql.DB, user handlers.UserHandler) error {
	_, err := DB.Exec("INSERT into users (text_id, login, created_at, password) VALUES(?, ?, ?, ?)", user.ID, user.Login, time.Now(), user.Password)
	return err
}

func UpdateUserOnline(DB *sql.DB, id string, online bool) error {
	onlineValue := 0
	if online {
		onlineValue = 1
	}
	query := `UPDATE users SET ONLINE = ? WHERE text_id = ?`
	_, err := DB.Exec(query, onlineValue, id)
	if err != nil {
		return err
	}
	return nil
}

func GetUserByLogin(DB *sql.DB, login string) (*handlers.UserHandler, error) {
	query := "SELECT text_id, login, password, online FROM users WHERE login = ?"
	row := DB.QueryRow(query, login)
	var user handlers.UserHandler

	err := row.Scan(&user.ID, &user.Login, &user.Password, &user.Online)
	if err != nil {
		return nil, err
	}
	return &user, err
}
