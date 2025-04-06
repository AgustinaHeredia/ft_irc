# ft_irc

`ft_irc` es un proyecto de la escuela 42 cuyo objetivo es implementar un servidor IRC (Internet Relay Chat) desde cero, utilizando C++98 y programación en red con sockets.

El proyecto reproduce las funcionalidades básicas del protocolo IRC descrito en la [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459), permitiendo la comunicación entre múltiples clientes a través de canales, mensajes privados y otros comandos clásicos.

---

## 📌 Objetivos

- Implementar un servidor IRC compatible con varios clientes conectados simultáneamente.
- Seguir el protocolo IRC utilizando sockets TCP y la llamada `poll()`.
- Modularizar los comandos del protocolo según el estándar de IRC.
- Gestionar usuarios, canales, operadores y modos correctamente.

---

## ⚙️ Requisitos técnicos

- Lenguaje: `C++98`
- OS: `Linux`
- Herramientas de red: `sockets`, `poll`, `fcntl`, `getaddrinfo`, etc.
- Sin uso de bibliotecas externas.

---

## 🚀 Funcionalidades implementadas

- **Conexión de múltiples clientes**
- **Autenticación con contraseña**
- **Comandos básicos:**
  - `NICK`, `USER`, `PASS`, `QUIT`
  - `JOIN`, `PART`
  - `PRIVMSG`, `NOTICE`
  - `PING`, `PONG`
- **Gestión de canales:**
  - Creación y destrucción dinámica
  - Operadores de canal
  - Temas con `TOPIC`
  - Listado de usuarios con `WHO`
- **Modos de canal y usuario (`MODE`):**
  - `+i / -i`: Canal por invitación
  - `+t / -t`: Solo operadores pueden cambiar el tema
  - `+k / -k`: Clave del canal
  - `+o / -o`: Añadir o quitar operadores
  - `+l / -l`: Límite de usuarios
- **Comandos avanzados:**
  - `INVITE`, `KICK`

> ❌ **Nota**: La funcionalidad de transferencia de archivos mediante DCC (Direct Client-to-Client) no fue incluida en esta versión.

---

## 🛠️ Compilación

```bash
make
```
🧪 Uso
```bash
./ircserv <port> <password>
```
Ejemplo:

```bash
./ircserv 6667 mySecretPass
```
Puedes conectarte con clientes IRC como:
```bash
nc 127.0.0.1 <puerto>

```
