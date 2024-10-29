EXEC = rocket
# We could specify a c++ standard here
CXXFLAGS = -g -Wall -O2 -lpthread 
CXX = g++

CXXFILES = $(wildcard *.cc)
OFILES   = $(CXXFILES:.cc=.o)

# We could also set capabilities to not run the executable as root
# More info here: https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base#capabilitiesrunning_the_app_with_rt_priority_as_a_non-root_user
$(EXEC): $(OFILES)
	$(CXX) $(CXXFLAGS) $(OFILES) -o $(EXEC)

depend:
	@echo "Updating dependencies..."
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CXX) -MM $(CXXFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

clean:
	@echo "Cleaning up..."
	@/bin/rm -f *.o *.cc~ *.h~ $(EXEC) $(OFILES)

# DO NOT DELETE THIS LINE
configure.o: configure.cc configure.h logging.h
logging.o: logging.cc logging.h
main.o: main.cc task.h logging.h configure.h
task.o: task.cc task.h
