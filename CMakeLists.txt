cmake_minimum_required(VERSION 3.10)
	project(dnwiki)
	option(MYCOMPDIR "Compile dnwiki" ON)
	add_executable(dnwiki src/main.c)

	target_link_libraries(dnwiki PUBLIC "-ljq -lm -lz -lcurl -lsox")
