f_id        f_config_name   f_config_value
----------  --------------  --------------
1           primary_server  218.56.36.175 
2           primary_dns     219.146.0.130 
3           primary_port    5513          
4           second_server   219.146.0.130 
5           second_dns      219.146.0.130 
6           second_port     9027          
7           gateway_id      37060002      
8           net_type        0             
9           md5_key         863CA654      
10          aes_key         d41d8cd98f00b2
11          collect_mode    0             
12          collect_cycle   15            
13          report_mode     0             
14          beat_cycle      2             
15          svr_num         0001          
16          appmd5          28a011de19210e

ip是218.56.36.175端口是5513，集中期37060002

.head on
.mode column

select * from t_base_define;
update t_base_define set f_config_value='218.56.36.175' where f_id=1;
update t_base_define set f_config_value='5513' where f_id=3;

update t_base_define set f_config_value='122.5.18.174' where f_id=1;
update t_base_define set f_config_value='9011' where f_id=3;
update t_base_define set f_config_value='9026' where f_id=3;
update t_base_define set f_config_value='37060001' where f_id=7;