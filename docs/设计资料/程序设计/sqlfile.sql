---------------------------------
--基本参数配置表
---------------------------------
drop table if exists "t_base_define"
;
create table t_base_define
(
	f_id	integer	primary key,
	f_config_name	varchar(16),
	f_config_value	varchar(50)
);

insert into t_base_define(f_id, f_config_name, f_config_value)values(1,	 'primary_server', '219.140.0.136');
insert into t_base_define(f_id, f_config_name, f_config_value)values(2,	 'primary_dns', '219.141.0.131');
insert into t_base_define(f_id, f_config_name, f_config_value)values(3,	 'primary_port', '4439');
insert into t_base_define(f_id, f_config_name, f_config_value)values(4,	 'second_server', '220.54.1.21');
insert into t_base_define(f_id, f_config_name, f_config_value)values(5,	 'second_dns', '213.5.1.2');
insert into t_base_define(f_id, f_config_name, f_config_value)values(6,	 'second_port', '9001');
insert into t_base_define(f_id, f_config_name, f_config_value)values(7,	 'gateway_id', '9403');
insert into t_base_define(f_id, f_config_name, f_config_value)values(8,	 'net_type', '0/1/2');
insert into t_base_define(f_id, f_config_name, f_config_value)values(9,	 'md5_key', '863CA654');
insert into t_base_define(f_id, f_config_name, f_config_value)values(10, 'aes_key', 'd41d8cd98f00b204e9800998ecf8427e');
insert into t_base_define(f_id, f_config_name, f_config_value)values(11, 'collect_mode', '0/1');
insert into t_base_define(f_id, f_config_name, f_config_value)values(12, 'collect_cycle', '15');
insert into t_base_define(f_id, f_config_name, f_config_value)values(13, 'report_mode', '0/1');
insert into t_base_define(f_id, f_config_name, f_config_value)values(14, 'beat_cycle', '2');

---------------------------------
--仪表地址信息配置表
---------------------------------
drop table if exists "t_meter_info"
;
create table t_meter_info
(
	f_id	integer primary key,
	f_meter_address	varchar(50),
	f_meter_type	integer,
	f_meter_channel	integer,
	f_install_pos	varchar(50),
	f_device_id	integer,
	f_meter_proto_type	integer
);

---------------------------------
--电表历史数据表
---------------------------------
drop table if exists "t_elect_data"
;
create table t_elect_data
(
  f_id	integer	primary key,
	f_meter_info_id	integer,
	f_timestamp	varchar(50),
	f_time	varchar(50),
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
	f_id	integer	primary key,
	f_meter_info_id	integer,
	f_timestamp	varchar(50),
	f_time	varchar(50),
	f_accum_flow	float
);

---------------------------------
--热量表历史数据表
---------------------------------
drop table if exists "t_heat_data"
;
create table t_heat_data
(
	f_id	integer	primary key,
	f_meter_info_id	integer,
	f_timestamp	varchar(50),
	f_time	varchar(50),
	f_accum_heat	float
);

---------------------------------
--仪表返回的数据项配置表
---------------------------------
drop table if exists "t_request_data"
;
create table t_request_data
(
	f_id	integer	primary key,
	f_meter_type	integer,
	f_item_index	integer,
	f_col_name	varchar(20),
	f_col_type	varchar(20)
);

---------------------------------
--时间点配置表
---------------------------------
drop table if exists "t_time_node"
;
create table t_time_node
(
	f_id	integer	primary key,
	f_time_name	varchar(20),
	f_time_node	varchar(100)
);

---------------------------------
--电表历史数据视图
---------------------------------
drop view if exists "v_elect_data"
;
create view v_elect_data as
  select address.*, elec_data.* from
  t_elect_data elec_data left join
  t_meter_info address on
  elec_data.f_meter_info_id = address.f_id
;

---------------------------------
--水表历史数据视图
---------------------------------
drop view if exists "v_water_data"
;
create view v_water_data as
  select address.*, water_data.* from
  t_water_data water_data left join
  t_meter_info address on
  water_data.f_meter_info_id = address.f_id
;

---------------------------------
--热量表历史数据视图
---------------------------------
drop view if exists "v_heat_data"
;
create view v_heat_data as
  select address.*, heat_data.* from
  t_heat_data heat_data left join
  t_meter_info address on
  heat_data.f_meter_info_id = address.f_id
;
