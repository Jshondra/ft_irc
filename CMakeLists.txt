cmake_minimum_required(VERSION 3.20)
project(our_IRC)

set(CMAKE_CXX_STANDARD 98)

add_executable(our_IRC includes/Client.hpp includes/IRC.hpp srcs/main.cpp srcs/Client.cpp
                srcs/IRC.cpp srcs/IRC_cmd.cpp includes/Channels.hpp srcs/Channels.cpp srcs/auth.cpp srcs/away.cpp
                srcs/privmsg.cpp srcs/request.cpp srcs/response.cpp srcs/utils.cpp)

#add_executable(our_IRC bot/srcs/main.cpp bot/srcs/Bot.cpp bot/srcs/Bot.hpp bot/srcs/Socket.cpp bot/srcs/Socket.hpp)