NAME = ircserv

SRCS = ./srcs/main.cpp \
       ./srcs/Server.cpp \
       ./srcs/ServerBot.cpp \
       ./srcs/ServerChanMode.cpp \
       ./srcs/ServerCheckChanClient.cpp \
       ./srcs/ServerGenMeme.cpp \
       ./srcs/ServerInvite.cpp \
       ./srcs/ServerJoin.cpp \
       ./srcs/ServerKick.cpp \
       ./srcs/ServerList.cpp \
       ./srcs/ServerMode.cpp \
       ./srcs/ServerMotd.cpp \
       ./srcs/ServerNames.cpp \
       ./srcs/ServerNick.cpp \
       ./srcs/ServerNotice.cpp \
       ./srcs/ServerPart.cpp \
       ./srcs/ServerPass.cpp \
       ./srcs/ServerPing.cpp \
       ./srcs/ServerPrivmsg.cpp \
       ./srcs/ServerQuit.cpp \
       ./srcs/ServerTopic.cpp \
       ./srcs/ServerUser.cpp \
       ./srcs/ServerUserMode.cpp \
       ./srcs/Chan.cpp \
       ./srcs/Client.cpp \
       ./srcs/Msg.cpp \
       ./srcs/utils.cpp

OBJS = $(SRCS:.cpp=.o)

DEPS = $(OBJS:.o=.d)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD

.cpp.o = $(CXX) $(CXXFLAGS) -c $< -o $(<:.cpp=.o)

all:	$(NAME)

debug: CXXFLAGS += -DDEBUG -g

debug: $(NAME)

$(NAME):	$(OBJS)
		$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re
