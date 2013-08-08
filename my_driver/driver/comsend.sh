make
if [ $? == 0 ]
then
echo "Sending file to 10.0.5.85"
sshpass -p password scp -r mydriver.ko root@10.0.5.85:/root/;
fi
