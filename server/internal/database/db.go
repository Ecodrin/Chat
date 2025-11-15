package database

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"slices"
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

func GetUserByID(DB *sql.DB, id string) (*handlers.UserHandler, error) {
	query := "SELECT text_id, login, password FROM users WHERE text_id = ?"
	row := DB.QueryRow(query, id)
	var user handlers.UserHandler

	err := row.Scan(&user.ID, &user.Login, &user.Password)
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
	if contactUser.ID == id {
		return fmt.Errorf("contactID == id")
	}
	user, err := GetUserByID(DB, id)
	if err != nil {
		return err
	}
	query := "SELECT contacts FROM users WHERE text_id = ?"
	var contacts []handlers.ContactHandler
	var contactsJSON string
	err = DB.QueryRow(query, id).Scan(&contactsJSON)
	if err != nil {
		return err
	}
	if contactsJSON != "" {
		err = json.Unmarshal([]byte(contactsJSON), &contacts)
		if err != nil {
			return err
		}
	}
	if slices.Contains(contacts, handlers.ContactHandler{ID: contactUser.ID}) {
		return fmt.Errorf("contact alreay in contacts")
	}
	query = `
        UPDATE users
        SET contacts = JSON_ARRAY_APPEND(contacts, '$', JSON_OBJECT('text_id', ?))
        WHERE text_id = ?
    `
	_, err = DB.Exec(query, contactUser.ID, user.ID)
	return err
}

func GetLoginContactsByID(DB *sql.DB, id string) ([]string, error) {
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

func GetIDContactsByID(DB *sql.DB, id string) ([]string, error) {
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
	contactsID := make([]string, len(contacts))
	for i := range contacts {
		user, err := GetUserByID(DB, contacts[i].ID)
		if err != nil {
			return nil, err
		}
		contactsID[i] = user.ID
	}
	return contactsID, nil
}

func DeleteContactByID(DB *sql.DB, id string, contact string) error {
	contactsLogins, err := GetLoginContactsByID(DB, id)
	if err != nil {
		return fmt.Errorf("database get login contacts by id %s", err.Error())
	}

	index := slices.Index(contactsLogins, contact)
	if index == -1 {
		return fmt.Errorf("unknown contact")
	}

	var contacts []handlers.ContactHandler
	query := "SELECT contacts FROM users WHERE text_id = ?"
	var contactsJSON []byte
	err = DB.QueryRow(query, id).Scan(&contactsJSON)
	if err != nil {
		return err
	}
	err = json.Unmarshal(contactsJSON, &contacts)
	if err != nil {
		return err
	}

	contacts = append(contacts[:index], contacts[(index+1):]...)

	contactsJSON, err = json.Marshal(contacts)
	if err != nil {
		return nil
	}

	query = "UPDATE users SET contacts = ? WHERE text_id = ?;"
	_, err = DB.Exec(query, contactsJSON, id)
	if err != nil {
		return err
	}
	return nil
}
