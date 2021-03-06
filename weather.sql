DROP TABLE IF EXISTS `measurements`;

create table measurements( 
  temp        FLOAT(4, 1), 
  humidity    FLOAT(4, 1) UNSIGNED,
  pressure    SMALLINT UNSIGNED, 
  date_time datetime not null, 
  primary key (date_time) 
) 
partition by list ( year(date_time)) 
( partition p_2013 VALUES in (2013), 
  partition p_2014 VALUES in (2014));
