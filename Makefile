
refresh:
	cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -S . -B build/
build: refresh
	make -C build
run: build
	./build/OpenGLTempl

.PHONY: build run refresh



