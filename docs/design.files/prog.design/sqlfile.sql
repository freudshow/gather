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

insert into t_base_define(f_id, f_config_name, f_config_value)values(1, 'primary_server', '122.5.18.174');
insert into t_base_define(f_id, f_config_name, f_config_value)values(2, 'primary_dns', '219.146.0.130');
insert into t_base_define(f_id, f_config_name, f_config_value)values(3, 'primary_port', '9026');
insert into t_base_define(f_id, f_config_name, f_config_value)values(4, 'second_server', '219.146.0.130');
insert into t_base_define(f_id, f_config_name, f_config_value)values(5, 'second_dns', '219.146.0.130');
insert into t_base_define(f_id, f_config_name, f_config_value)values(6, 'second_port', '9027');
insert into t_base_define(f_id, f_config_name, f_config_value)values(7, 'gateway_id', '5001');
insert into t_base_define(f_id, f_config_name, f_config_value)values(8, 'net_type', '0');-- 网络类型. 0 - GPRS网络; 1 - 485网络; 2 - LAN
insert into t_base_define(f_id, f_config_name, f_config_value)values(9, 'md5_key', '863CA654');
insert into t_base_define(f_id, f_config_name, f_config_value)values(10, 'aes_key', 'd41d8cd98f00b204e9800998ecf8427e');
insert into t_base_define(f_id, f_config_name, f_config_value)values(11, 'collect_mode', '0');-- 0-根据collect_cycle周期抄表, 1-根据[t_time_node].[f_time_node]定点采集。
insert into t_base_define(f_id, f_config_name, f_config_value)values(12, 'collect_cycle', '15');
insert into t_base_define(f_id, f_config_name, f_config_value)values(13, 'report_mode', '0');-- 数据上报模式. 0 - 主动上报; 1 - 被动请求.
insert into t_base_define(f_id, f_config_name, f_config_value)values(14, 'beat_cycle', '2');
insert into t_base_define(f_id, f_config_name, f_config_value)values(15, 'svr_num', '0001');
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

insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values ('20', 1, '11110021147872', 1, 0, '6#管道井');--德鲁热表
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values ('20', 2, '11110021147872', 1, 0, '5#管道井');--德鲁热表
insert into t_meter_info (f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos) values ('20', 3, '11110021147872', 1, 0, '3#地下室');--德鲁热表

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
	f_pact_tot_elec varchar(50),
	f_nact_tot_elec varchar(50),
	f_preact_tot_elec varchar(50),
	f_nreact_tot_elec varchar(50),
	f_act_tot_elec varchar(50),
	f_react_tot_elec varchar(50),
	f_ovr_pow_fac varchar(50),
	f_pmax_dem varchar(50)
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
	f_cur_accum_flow varchar(50),
	f_flowrate varchar(50),
	f_settle_date_accum_flow varchar(50),
	f_settle_date varchar(50),
	f_accum_work_time varchar(50)
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
	f_cur_cold_E	varchar(50), 
	f_cur_heat_E	varchar(50), 
	f_heat_power	varchar(50), 
	f_flowrate	varchar(50), 
	f_accum_flow	varchar(50), 
	f_in_temp	varchar(50), 
	f_out_temp	varchar(50), 
	f_accum_work_time	varchar(50), 
	f_cur_time	varchar(50), 
	f_state	varchar(50)
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

insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(1, '20', '01', 'f_cur_cold_E', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(2, '20', '02', 'f_cur_heat_E', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(3, '20', '03', 'f_heat_power', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(4, '20', '04', 'f_flowrate', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(5, '20', '05', 'f_accum_flow', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(6, '20', '06', 'f_in_temp', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(7, '20', '07', 'f_out_temp', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(8, '20', '08', 'f_accum_work_time', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(9, '20', '09', 'f_cur_time', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(10, '20', '10', 'f_state', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(11, '10', '01', 'f_cur_accum_flow', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(12, '10', '02', 'f_flowrate', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(13, '10', '03', 'f_settle_date_accum_flow', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(14, '10', '04', 'f_settle_date', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(15, '10', '05', 'f_accum_work_time', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(16, '40', '01', 'f_pact_tot_elec', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(17, '40', '02', 'f_nact_tot_elec', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(18, '40', '03', 'f_preact_tot_elec', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(19, '40', '04', 'f_nreact_tot_elec', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(20, '40', '05', 'f_act_tot_elec', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(21, '40', '06', 'f_react_tot_elec', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(22, '40', '07', 'f_ovr_pow_fac', 'varchar(50)');
insert into t_request_data(f_id, f_meter_type, f_item_index, f_col_name, f_col_type)values(23, '40', '08', 'f_pmax_dem', 'varchar(50)');

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
