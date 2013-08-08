make clean all
if [ $? == 0 ]
then
echo "Sending file to 10.0.5.116"
sshpass -p password scp -r bin/* root@10.0.5.116:/root/;
fi
