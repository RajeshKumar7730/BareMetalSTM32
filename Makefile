
all:bootloader app	
	@mkdir -p fw_images/app
	@mkdir -p fw_images/bootloader
	@cp build/bare-metal-stm32* fw_images/app
	@cp ___Bootloader/build/bootloader* fw_images/bootloader

app:gen_protobuf_headers
	@cmake -B build -S .
	@cmake --build build
	python3 scripts/pad_metadata.py
	
bootloader:
	@cmake -B ___Bootloader/build -S ___Bootloader/.
	cmake --build ___Bootloader/build

gen_protobuf_headers:
	@mkdir -p protobuf/c_gen
	@mkdir -p protobuf/py_gen
	@protoc -I protobuf -I protobuf/msgs/   --plugin="python3 nanopb_generator.py"  --nanopb_out=protobuf/c_gen/  --proto_path=protobuf/nanopb/generator/proto/ protobuf/msgs/my_msgs.proto 	
	@protoc -I protobuf --python_out=protobuf/py_gen  --proto_path=protobuf/nanopb/generator/proto/ protobuf/msgs/my_msgs.proto

clean:
	@rm -rf build/ protobuf/c_gen protobuf/py_gen ___Bootloader/build fw_images


gdb:
	gdb-multiarch build/bare-metal-stm32.elf \
  			-ex "target remote host.docker.internal:3333" \
			  -ex "load" \
			  	-ex 'set $$pc = 0x200003dc' \
					-ex "break main"

debug:
	openocd -f ./platform/openocd.cfg 

st-connect:
	@sudo modprobe vhci_hcd
	@powershell.exe usbipd attach --wsl --busid 1-2

st-disconnect:
	@powershell.exe usbipd detach --busid 1-2

run-docker:
	@docker run -it -v ".:/workspace" -v "C:\Users\rajes\.ssh:/root/.ssh" myimg:v3

terminal:
	@docker exec -it stm32-dev /bin/bash



#export PATH=$PATH:/workspace/protobuf/nanopb/generator