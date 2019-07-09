all:clientTest serverTest  perpare_virtual_ip create_batch_cmd
clientTest:
	g++ clientTest.cpp -o output/clientTest -std=c++11
serverTest:
	g++ serverTest.cpp -o output/serverTest -std=c++11
perpare_virtual_ip:
	bash perpare_virtual_ip.sh 80 "enp0s3, enp0s8, enp0s9, enp0s10"
create_batch_cmd:
	bash create_batch_cmd.sh
