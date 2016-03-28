---------------------------------
--基本参数配置表
---------------------------------
drop table if exists "t_base_define"
;
create table t_base_define
(
	f_id	integer	primary key autoincrement,
	f_config_name	varchar(16),
	f_config_value	varchar(50)
);

insert into t_base_define(f_config_name, f_config_value)values('primary_server', '122.5.18.174');
insert into t_base_define(f_config_name, f_config_value)values('primary_dns', '219.146.0.130');
insert into t_base_define(f_config_name, f_config_value)values('primary_port', '9026');
insert into t_base_define(f_config_name, f_config_value)values('second_server', '219.146.0.130');
insert into t_base_define(f_config_name, f_config_value)values('second_dns', '219.146.0.130');
insert into t_base_define(f_config_name, f_config_value)values('second_port', '9027');
insert into t_base_define(f_config_name, f_config_value)values('gateway_id', '5001');
insert into t_base_define(f_config_name, f_config_value)values('net_type', '0/1/2');
insert into t_base_define(f_config_name, f_config_value)values('md5_key', '863CA654');
insert into t_base_define(f_config_name, f_config_value)values('aes_key', 'd41d8cd98f00b204e9800998ecf8427e');
insert into t_base_define(f_config_name, f_config_value)values('collect_mode', '0/1');
insert into t_base_define(f_config_name, f_config_value)values('collect_cycle', '15');
insert into t_base_define(f_config_name, f_config_value)values('report_mode', '0/1');
insert into t_base_define(f_config_name, f_config_value)values('beat_cycle', '2');
---------------------------------
--仪表地址信息配置表
---------------------------------
drop table if exists "t_meter_info"
;
create table t_meter_info
(
	f_id	integer primary key autoincrement,
	f_meter_type	varchar(2),
	f_device_id	varchar(4),
	f_meter_address	varchar(14),
	f_meter_channel	varchar(2),	
	f_meter_proto_type	varchar(2),
	f_install_pos	varchar(50)
);

insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (10, 1, '11110021147872', 1, 0, '6#管道井');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (10, 2, '11110052310292', 7, 1, '5#管道井');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (20, 3, '11110021147872', 1, 0, '3#地下室');--德鲁热表
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (20, 4, '11110053110202', 0, 3, '3#地下室');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (40, 5, '1', 7, 6, '2#电表间');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (40, 5, '12', 7, 6, '2#电表间');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (40, 5, '123', 7, 6, '2#电表间');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (40, 5, '1234', 7, 6, '2#电表间');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (40, 5, '12345', 7, 6, '2#电表间');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (40, 6, '123456', 7, 6, '1#电表间');
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values (40, 6, '1234567', 7, 6, '1#电表间');
---------------------------------
--电表历史数据表
---------------------------------
drop table if exists "t_elect_data"
;
create table t_elect_data
(
	f_id	integer	primary key autoincrement,
	f_meter_type	integer,
	f_device_id	integer,
	f_meter_address	varchar(50),
	f_timestamp	timestamp,--时间戳（'yyyy-MM-dd HH:mm:ss.zzz')
	f_time	timestamp,--抄表时间点（'yyyy-MM-dd HH:mm:ss.zzz')
	f_total_active_power	float,
	f_maximum_demand	float,
	f_overall_power_factor	float
);

---------------------------------
--水表历史数据表
---------------------------------
drop table if exists "t_water_data"
;
create table t_water_data
(
	f_id	integer	primary key autoincrement,
	f_meter_type	integer,
	f_device_id	integer,
	f_meter_address	varchar(50),
	f_timestamp	timestamp,--时间戳（'yyyy-MM-dd HH:mm:ss.zzz')
	f_time	timestamp,--抄表时间点（'yyyy-MM-dd HH:mm:ss.zzz')
	f_accum_flow	float
);

---------------------------------
--热量表历史数据表
---------------------------------
drop table if exists "t_heat_data"
;
create table t_heat_data
(
	f_id	integer	primary key autoincrement,
	f_meter_type	integer,
	f_device_id	integer,
	f_meter_address	varchar(50),
	f_timestamp	timestamp,--时间戳（'yyyy-MM-dd HH:mm:ss.zzz')
	f_time	timestamp,--抄表时间点（'yyyy-MM-dd HH:mm:ss.zzz')
	f_cur_cold_E	varchar(10), 
	f_cur_heat_E	varchar(10), 
	f_heat_power	varchar(10), 
	f_flowrate	varchar(10), 
	f_accum_flow	varchar(10), 
	f_in_temp	varchar(6), 
	f_out_temp	varchar(6), 
	f_accum_work_time	varchar(10), 
	f_cur_time	varchar(14), 
	f_state	varchar(4)
);

---------------------------------
--仪表需要返回的数据项配置表
---------------------------------
drop table if exists "t_request_data"
;
create table t_request_data
(
	f_id	integer	primary key autoincrement,
	f_meter_type	varchar(2),
	f_item_index	varchar(2),
	f_col_name	varchar(20),
	f_col_type	varchar(20)
);

insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '01', 'f_cur_cold_E', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '02', 'f_cur_heat_E', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '03', 'f_heat_power', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '04', 'f_flowrate', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '05', 'f_accum_flow', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '06', 'f_in_temp', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '07', 'f_out_temp', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '08', 'f_accum_work_time', 'varchar(20)');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '09', 'f_cur_time', 'varchar(20)');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('20', '10', 'f_state', 'varchar(20)');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('10', '01', 'f_cur_accum_flow', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('10', '02', 'f_flowrate', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('10', '03', 'f_settle_date_accum_flow', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('10', '04', 'f_settle_date', 'varchar(20)');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('10', '05', 'f_accum_work_time', 'varchar(20)');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('40', '01', 'f_active_total_electricity', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('40', '02', 'f_cur_voltage', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('40', '03', 'f_cur_current', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('40', '04', 'f_cur_active_power', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('40', '05', 'f_cur_freq', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('40', '06', 'f_cur_reactive_power', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('40', '07', 'f_cur_ apparent_power', 'float');
insert into t_request_data(f_meter_type, f_item_index, f_col_name, f_col_type)values('40', '08', 'f_cur_power_factor', 'float');
---------------------------------
--时间点配置表
---------------------------------
drop table if exists "t_time_node"
;
create table t_time_node
(
	f_id	integer	primary key autoincrement,
	f_time_name	varchar(20),
	f_time_node	varchar(100)
);
