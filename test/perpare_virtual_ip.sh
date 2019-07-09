#!/bin/sh
vip_total_count=$1
arg_interface_list=$2
OLD_IFS="$IFS"
IFS=","
net_interface_list=($arg_interface_list)
IFS="$OLD_IFS"
for interface in ${net_interface_list[@]}
do
	echo "interface:$interface"
done


net_interface_count=${#net_interface_list[@]}
net_interface_vip_count=()
vip_count_index=0
while [ $vip_count_index -lt $net_interface_count ]
do
	interface_item=${net_interface_list[$vip_count_index]}
	net_interface_vip_count[$vip_count_index]=`ifconfig $interface_item | wc -l`
	vip_count_index=`expr $vip_count_index + 1`
done

interface_item=${net_interface_list[0]}
start_ip=`ifconfig $interface_item | grep "inet" | egrep "netmask|Mask" | awk '{print $2}'`
mask=`ifconfig $interface_item | grep "inet" | egrep "netmask|Mask" | awk '{print $4}'`
ip_flag=`expr index "$start_ip" :`
mask_flag=`expr index "$mask" :`
if [ $ip_flag -gt 0 ]
then
	start_ip=`echo $start_ip|awk -F ':' '{print $2}'`
fi
if [ $mask_flag -gt 0 ]
then
	mask=`echo $mask|awk -F ':' '{print $2}'`
fi

echo "interface_item:"$interface_item
echo "stat_ip:"$start_ip
echo "mask:"$mask
ip_part_1=`echo $start_ip|awk -F '.' '{print $1}'`
ip_part_2=`echo $start_ip|awk -F '.' '{print $2}'`
ip_part_3=`echo $start_ip|awk -F '.' '{print $3}'`
ip_part_4=`echo $start_ip|awk -F '.' '{print $4}'`


`ifconfig -a | grep "inet " | awk '{print $2}'> output/local_ip_info_tmp.txt`


echo '' > output/exist_local_ip.txt
echo '' > output/add_new_virtual_ip.sh
echo '' > output/new_virtual_ip.txt
truncate -s 0 output/exist_local_ip.txt
truncate -s 0 output/add_new_virtual_ip.sh
truncate -s 0 output/new_virtual_ip.txt
for line in `cat output/local_ip_info_tmp.txt`
do
	ip_flag=`expr index "$line" :`
	if [ $ip_flag -gt 0 ]
	then
		ip=`echo $line|awk -F ':' '{print $2}'`
        else
		ip=$line
	fi

	if [ "$ip"x = "127.0.0.1"x ]
	then
		continue
	fi

	echo "$ip" >> output/exist_local_ip.txt
done
rm output/local_ip_info_tmp.txt

local_ip_count=`cat output/exist_local_ip.txt | wc -l`
vip_total_count=`expr $vip_total_count - $local_ip_count`


count=0
cmdCount=0
while [ $cmdCount -lt $vip_total_count ]
do
	count=`expr $count + 1`
	ip_to_try=$ip_part_1.$ip_part_2.$ip_part_3.$count
	is_exist=$(ping -c 1 -w 1 $ip_to_try|grep -q 'ttl=' && echo 1 || echo 0)
	echo 'ip_to_try:'$ip_to_try' is_exist:'$is_exist
	if [ $is_exist -eq 0 ]
	then
		interface_index=$(($cmdCount%$net_interface_count))
		net_interface_vip_count[$interface_index]=`expr ${net_interface_vip_count[$interface_index]} + 1`

		while [ 0 -lt 1 ]
		do
			name_exist=`ifconfig ${net_interface_list[$interface_index]}:${net_interface_vip_count[$interface_index]} | grep "inet "| wc -l`
			if [ $name_exist -gt 0 ]
			then
				net_interface_vip_count[$interface_index]=`expr ${net_interface_vip_count[$interface_index]} + 1`
			else
				break
			fi
		done
		echo "ifconfig ${net_interface_list[$interface_index]}:${net_interface_vip_count[$interface_index]} $ip_to_try netmask $mask up &" >> output/add_new_virtual_ip.sh
		#echo "ifconfig ${net_interface_list[$interface_index]}:${net_interface_vip_count[$interface_index]} $ip_to_try netmask $mask up &" >> add_new_virtual_ip.history
		echo $ip_to_try >> output/new_virtual_ip.txt
		cmdCount=`expr $cmdCount + 1`
	fi
done


