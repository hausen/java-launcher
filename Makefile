# jar file to load
JARFILE=filename.jar
EXECUTABLE=java-launcher
OBJS=main.o process.o
CXXFLAGS=-O0 # IMPORTANT: don't optimimize the code
# uncomment the following line to create a statically linked executable
#STATIC=-static

all: $(EXECUTABLE)

# uncomment embedded_jar.o to embed the jar into the executable
# also, uncomment -DEMBEDDED_JAR_FILE in the main.o entry
$(EXECUTABLE): $(OBJS) # embedded_jar.o
	$(CXX) $(STATIC) $(CXXFLAGS) $^ -o $(EXECUTABLE)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) "-DJARFILE=\"$(JARFILE)\"" $< -c -o $@ # -DEMBEDDED_JAR_FILE

embedded_jar.o: $(JARFILE)
	rm -f embedded_jar_file
	ln -s "$(JARFILE)" embedded_jar_file
	ld -r -b binary embedded_jar_file -o $@
	rm embedded_jar_file

clean:
	rm -rf $(OBJS) $(EXECUTABLE) embedded_jar.o
