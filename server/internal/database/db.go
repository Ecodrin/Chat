package database

import (
	"database/sql"
	"encoding/json"
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
	_, err := DB.Exec("INSERT into users (text_id, login, created_at, password, online) VALUES(?, ?, ?, ?, ?)", user.ID, user.Login, time.Now(), user.Password, user.Online)
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

func GetUserByID(DB *sql.DB, id string) (*handlers.UserHandler, error) {
	query := "SELECT text_id, login, password, online FROM users WHERE text_id = ?"
	row := DB.QueryRow(query, id)
	var user handlers.UserHandler

	err := row.Scan(&user.ID, &user.Login, &user.Password, &user.Online)
	if err != nil {
		return nil, err
	}
	return &user, err
}

func AddContactByID(DB *sql.DB, id string, contact string) error {
	err := UpdateContactsNull(DB)
	if err != nil {
		return err
	}
	contactUser, err := GetUserByLogin(DB, contact)
	if err != nil {
		return err
	}
	user, err := GetUserByID(DB, id)
	if err != nil {
		return err
	}
	query := `
        UPDATE users
        SET contacts = JSON_ARRAY_APPEND(contacts, '$', JSON_OBJECT('text_id', ?))
        WHERE text_id = ?
    `
	_, err = DB.Exec(query, contactUser.ID, user.ID)
	return err
}

func GetContactsByID(DB *sql.DB, id string) ([]string, error) {
	err := UpdateContactsNull(DB)
	if err != nil {
		return nil, err
	}

	query := "SELECT contacts FROM users WHERE text_id = ?"
	var contacts []handlers.ContactHandler
	var contactsJSON string
	err = DB.QueryRow(query, id).Scan(&contactsJSON)
	if err != nil {
		return nil, err
	}
	if contactsJSON != "" {
		err = json.Unmarshal([]byte(contactsJSON), &contacts)
		if err != nil {
			return nil, err
		}
	}
	contactsLogins := make([]string, len(contacts))
	for i := range contacts {
		user, err := GetUserByID(DB, contacts[i].ID)
		if err != nil {
			return nil, err
		}
		contactsLogins[i] = user.Login
	}
	return contactsLogins, nil
}
