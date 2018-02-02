sv_in := include/network/client_class.cpp \
		 include/network/network.hpp \
		 include/network/server_api.hpp \
		 include/network/protocol_message.cpp \
		 include/database/database.cpp \
		 include/utility/logged_val.cpp

cl_in := include/network/protocol_message.cpp \
		 include/client/network/messages.cpp 
cl_sh := -lboost_system -lboost_filesystem
sv_sh := -lboost_system -lboost_filesystem -lboost_regex -lpqxx
warnings := 

all:
	g++ $(warnings) client/main.cpp $(cl_in) -pthread  $(cl_sh) -I include/ -o client-app
	g++ $(warnings) server/main.cpp -g $(sv_in) -pthread  $(sv_sh) -I include/ -o server-app
clean:
	rm client-app server-app

runserv:
	valgrind --leak-check=no -v ./server-app