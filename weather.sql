DROP TABLE IF EXISTS `measurements`;

CREATE TABLE `measurements` (
  `temp` tinyint(4) DEFAULT NULL,
  `humidity` tinyint(4) DEFAULT NULL,
  `pressure` tinyint(4) DEFAULT NULL,
  `date_time` datetime NOT NULL,
  PRIMARY KEY (`date_time`)
)
