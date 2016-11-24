-- phpMyAdmin SQL Dump
-- version 4.4.10
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Nov 23, 2016 at 10:49 AM
-- Server version: 5.6.25
-- PHP Version: 5.6.25

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `online_market`
--

DELIMITER $$
--
-- Procedures
--
CREATE DEFINER=`root`@`localhost` PROCEDURE `getUserData`(IN `user_id` INT)
BEGIN
DECLARE userTypeID INT;
DECLARE userTypeTableName TEXT;
    SELECT user_type into userTypeID FROM users WHERE users._id = user_id LIMIT 1;
	SELECT name into userTypeTableName FROM user_type WHERE user_type.type_id = userTypeID LIMIT 1;
	SET @sql_text = concat('select user.*, sp_user.* from users user JOIN ',userTypeTableName,' sp_user ON user._id = sp_user.user_id WHERE user._id = ', user_id);
	PREPARE stmt FROM @sql_text;
	EXECUTE stmt;
    DEALLOCATE PREPARE stmt;
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `accountants`
--

CREATE TABLE IF NOT EXISTS `accountants` (
  `user_id` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

--
-- Dumping data for table `accountants`
--

INSERT INTO `accountants` (`user_id`) VALUES
(75),
(100),
(103),
(104),
(105),
(107),
(108),
(109),
(110),
(111),
(114);

-- --------------------------------------------------------

--
-- Table structure for table `admins`
--

CREATE TABLE IF NOT EXISTS `admins` (
  `user_id` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

--
-- Dumping data for table `admins`
--

INSERT INTO `admins` (`user_id`) VALUES
(76);

-- --------------------------------------------------------

--
-- Table structure for table `availability_status`
--

CREATE TABLE IF NOT EXISTS `availability_status` (
  `_id` int(11) NOT NULL,
  `status` varchar(100) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `availability_status`
--

INSERT INTO `availability_status` (`_id`, `status`) VALUES
(1, 'available'),
(2, 'unavailable');

-- --------------------------------------------------------

--
-- Table structure for table `buyers`
--

CREATE TABLE IF NOT EXISTS `buyers` (
  `user_id` int(11) NOT NULL,
  `address` text NOT NULL,
  `creditcard` varchar(25) NOT NULL,
  `cc_ccv` int(3) NOT NULL,
  `cc_month` int(2) NOT NULL,
  `cc_year` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

--
-- Dumping data for table `buyers`
--

INSERT INTO `buyers` (`user_id`, `address`, `creditcard`, `cc_ccv`, `cc_month`, `cc_year`) VALUES
(72, 'b Address22bbb', '22222222', 222, 2, 2023),
(82, '6B, Pyramids Gardens', '763421389103', 866, 10, 2019);

-- --------------------------------------------------------

--
-- Table structure for table `cart_items`
--

CREATE TABLE IF NOT EXISTS `cart_items` (
  `_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `product_id` int(11) NOT NULL,
  `quantity` int(11) NOT NULL DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

-- --------------------------------------------------------

--
-- Table structure for table `categories`
--

CREATE TABLE IF NOT EXISTS `categories` (
  `_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `categories`
--

INSERT INTO `categories` (`_id`, `name`) VALUES
(1, 'C1');

-- --------------------------------------------------------

--
-- Table structure for table `categories_spec`
--

CREATE TABLE IF NOT EXISTS `categories_spec` (
  `_id` int(11) NOT NULL,
  `category_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

-- --------------------------------------------------------

--
-- Table structure for table `deliverymen`
--

CREATE TABLE IF NOT EXISTS `deliverymen` (
  `user_id` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

--
-- Dumping data for table `deliverymen`
--

INSERT INTO `deliverymen` (`user_id`) VALUES
(77),
(101),
(113);

-- --------------------------------------------------------

--
-- Table structure for table `deliveryrequests`
--

CREATE TABLE IF NOT EXISTS `deliveryrequests` (
  `_id` int(11) NOT NULL,
  `order_id` int(11) NOT NULL,
  `deliveryman_id` int(11) NOT NULL,
  `duedate` date NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `deliveryrequests`
--

INSERT INTO `deliveryrequests` (`_id`, `order_id`, `deliveryman_id`, `duedate`) VALUES
(1, 1, 77, '2016-11-25');

-- --------------------------------------------------------

--
-- Table structure for table `orders`
--

CREATE TABLE IF NOT EXISTS `orders` (
  `_id` int(11) NOT NULL,
  `buyer_id` int(11) NOT NULL,
  `date` date NOT NULL,
  `cost` int(11) NOT NULL,
  `status_id` int(11) NOT NULL,
  `issuedate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `orders`
--

INSERT INTO `orders` (`_id`, `buyer_id`, `date`, `cost`, `status_id`, `issuedate`) VALUES
(1, 72, '2016-11-22', 2000, 1, '2016-11-21 09:26:41');

-- --------------------------------------------------------

--
-- Table structure for table `order_items`
--

CREATE TABLE IF NOT EXISTS `order_items` (
  `order_id` int(11) NOT NULL,
  `product_id` int(11) NOT NULL,
  `seller_id` int(11) NOT NULL,
  `quantity` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

-- --------------------------------------------------------

--
-- Table structure for table `order_status`
--

CREATE TABLE IF NOT EXISTS `order_status` (
  `_id` int(11) NOT NULL,
  `status` varchar(15) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `order_status`
--

INSERT INTO `order_status` (`_id`, `status`) VALUES
(4, 'delivered'),
(1, 'pending'),
(2, 'picked'),
(3, 'shipped');

-- --------------------------------------------------------

--
-- Table structure for table `products`
--

CREATE TABLE IF NOT EXISTS `products` (
  `_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `price` decimal(10,0) NOT NULL,
  `rate` decimal(11,0) NOT NULL,
  `size` varchar(20) NOT NULL,
  `weight` decimal(10,0) NOT NULL,
  `availability_id` int(1) NOT NULL,
  `available_quantity` int(10) NOT NULL,
  `origin` varchar(25) NOT NULL,
  `provider` varchar(25) NOT NULL,
  `image` text NOT NULL,
  `seller_id` int(11) NOT NULL,
  `category_id` int(11) NOT NULL,
  `solditems` int(11) NOT NULL,
  `earnings` int(11) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `products`
--

INSERT INTO `products` (`_id`, `name`, `price`, `rate`, `size`, `weight`, `availability_id`, `available_quantity`, `origin`, `provider`, `image`, `seller_id`, `category_id`, `solditems`, `earnings`) VALUES
(1, 'Product X', '100', '4', '100', '120', 1, 99, 'Apple', 'Samir', 'www.apple.com', 73, 1, 287, 2000);

-- --------------------------------------------------------

--
-- Table structure for table `product_spec`
--

CREATE TABLE IF NOT EXISTS `product_spec` (
  `_id` int(11) NOT NULL,
  `product_id` int(11) NOT NULL,
  `categories_spec_id` int(11) NOT NULL,
  `value` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

-- --------------------------------------------------------

--
-- Table structure for table `rates`
--

CREATE TABLE IF NOT EXISTS `rates` (
  `_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `product_id` int(11) NOT NULL,
  `rate` decimal(11,0) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

-- --------------------------------------------------------

--
-- Table structure for table `sellers`
--

CREATE TABLE IF NOT EXISTS `sellers` (
  `user_id` int(11) NOT NULL,
  `address` text NOT NULL,
  `bankaccount` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf32;

--
-- Dumping data for table `sellers`
--

INSERT INTO `sellers` (`user_id`, `address`, `bankaccount`) VALUES
(73, 's Adderss', 'sssBankAccountasd');

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `email` varchar(50) NOT NULL,
  `pass` text NOT NULL,
  `tel` varchar(25) NOT NULL,
  `user_type` int(11) NOT NULL,
  `user_status` int(11) NOT NULL DEFAULT '1'
) ENGINE=InnoDB AUTO_INCREMENT=115 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `users`
--

INSERT INTO `users` (`_id`, `name`, `email`, `pass`, `tel`, `user_type`, `user_status`) VALUES
(72, 'bbb', 'b@b.b', '$2y$11$PDgZfzAFRmrLkqUP3J7XWONhOw9oYGl3ik2UXNd88vHxOFZIMXq/G', '22222222', 1, 1),
(73, 'sssADSSS', 's@s.s', '$2y$11$7kRXHWZo8FEWojDrlKRtPeGoAgTUG3ybtTHfHBiD8h2PuBQUz2qU.', '33333333', 2, 1),
(75, 'aaa', 'a@a.a', '$2y$11$OxR6IxShh0n/AUuyUm0dU.GyKVkYAWqm.P7JhYkjLsj2EJkbXKZbW', '111', 3, 1),
(76, 'ddd23', 'd@d.d', '$2y$11$MRWnSo1lcaLBYhtcA0QbS.tMsoiavJJzw4XeD4nqNE3EOtfKwhOyS', '44444', 4, 1),
(77, 'mmm2', 'm@m.m', '$2y$11$81DCSoJZmFn1my4lMeFecufbu16/hJusz5fgCafGO05wy7.DEY2WW', '213124123123aa', 5, 1),
(82, 'Ibrahim Radwn', 'i.radwan1996@gmail.com', '$2y$11$tCijn/Sl2p9QSeuZ6/QojOIdlfpa3xi4bnlfm4GhkP4NgN3fAN2Wq', '00201097799856', 1, 2),
(100, 'emp', 'ds@ds.dsd', '$2y$11$oMTEoXeQ5aIwjY./0EjqTe/mo5NQUHPDCkgg3OY7fgEJ0mBLZTPiK', '0', 3, 1),
(101, 'emp', 'dddd@dddd.dddd', '$2y$11$bSrB4aJmecIoaIF.ErvT2..tcdhuB30ch9w1aAzJUCnpozvT.WLSe', '0', 5, 1),
(103, 'Emp', 'ac@ac.ac', '$2y$11$EqQWhxDiZ0YiaGpGu6E.OOPimESg/6QqqsxR0b8fxDFL3OB52KZMK', '0', 3, 1),
(104, 'Emp', 'acc@acc.acc', '$2y$11$gJB54t9FccB4bXLysY/hrObI4QCNesIZMsKy4J.F5yq6KkXOKru5.', '0', 3, 1),
(105, 'Emp', 'dsd@dsd.dsd', '$2y$11$mInxEa2ZjvCmi.OTeNgO4uTMLEYkqu8Umy3VHvV59h05fWd.haZga', '0', 3, 1),
(107, 'Emp', 'cxz@cz.xc', '$2y$11$7nMdD5lLXSwkZPiq0AHbKOvJSc86EEe4CMiPZCEjezVqpnZM966YS', '0', 3, 1),
(108, 'Emp', 'asd@dasd.dasd', '$2y$11$A78fmD9vTTcWuCIOCAbkHul6GbasdeaCtXnXa9m0XVd6bXL5kBFSy', '0', 3, 1),
(109, 'Emp', 'sfd@asd.asd', '$2y$11$Gw6A6FvNJr/2tYic3LOOHuny6OPkk53sIRxXeeTNQwL/ZuPbaMSDy', '0', 3, 1),
(110, 'Emp', 'asdf@asd.dqd', '$2y$11$Ivpm4YP7oGbWZPkZtAu..ehftwnu5bfeKO5HAI6gbPvtNzRTz9Ap.', '0', 3, 1),
(111, 'Emp', 'asd@asd.dsad', '$2y$11$im9.Ex5NetZ5gRWiyqxUuOzeet/NpfYfmuxha2aTunXCDFwL6O35K', '0', 3, 1),
(113, 'Emp', 'sfkdjghskjg@asd.asd', '$2y$11$eIZQwDobHb1zHLWjwIQl9.XAs2y4EUoUxUXG2P2eDJnbv00h02a0K', '0', 5, 1),
(114, 'VVVV', 'v@v.v', '$2y$11$X8lgUzKTKMh8i6iIOBBRiu55mLkc4zhjjEcTyHpS6TpKbBON.lGtm', '0123', 3, 1);

-- --------------------------------------------------------

--
-- Table structure for table `user_status`
--

CREATE TABLE IF NOT EXISTS `user_status` (
  `status_id` int(11) NOT NULL,
  `status_name` varchar(10) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `user_status`
--

INSERT INTO `user_status` (`status_id`, `status_name`) VALUES
(1, 'active'),
(2, 'banned');

-- --------------------------------------------------------

--
-- Table structure for table `user_type`
--

CREATE TABLE IF NOT EXISTS `user_type` (
  `type_id` int(11) NOT NULL,
  `name` varchar(11) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf32;

--
-- Dumping data for table `user_type`
--

INSERT INTO `user_type` (`type_id`, `name`) VALUES
(3, 'accountants'),
(4, 'admins'),
(1, 'buyers'),
(5, 'deliverymen'),
(2, 'sellers');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `accountants`
--
ALTER TABLE `accountants`
  ADD PRIMARY KEY (`user_id`);

--
-- Indexes for table `admins`
--
ALTER TABLE `admins`
  ADD PRIMARY KEY (`user_id`);

--
-- Indexes for table `availability_status`
--
ALTER TABLE `availability_status`
  ADD PRIMARY KEY (`_id`);

--
-- Indexes for table `buyers`
--
ALTER TABLE `buyers`
  ADD PRIMARY KEY (`user_id`),
  ADD UNIQUE KEY `user_id` (`user_id`);

--
-- Indexes for table `cart_items`
--
ALTER TABLE `cart_items`
  ADD PRIMARY KEY (`_id`),
  ADD UNIQUE KEY `user_id_2` (`user_id`,`product_id`),
  ADD KEY `user_id` (`user_id`),
  ADD KEY `product_id` (`product_id`);

--
-- Indexes for table `categories`
--
ALTER TABLE `categories`
  ADD PRIMARY KEY (`_id`),
  ADD UNIQUE KEY `name` (`name`);

--
-- Indexes for table `categories_spec`
--
ALTER TABLE `categories_spec`
  ADD PRIMARY KEY (`_id`,`category_id`,`name`),
  ADD UNIQUE KEY `category_id_2` (`category_id`,`name`),
  ADD KEY `category_id` (`category_id`);

--
-- Indexes for table `deliverymen`
--
ALTER TABLE `deliverymen`
  ADD PRIMARY KEY (`user_id`),
  ADD KEY `user_id` (`user_id`);

--
-- Indexes for table `deliveryrequests`
--
ALTER TABLE `deliveryrequests`
  ADD PRIMARY KEY (`_id`,`order_id`),
  ADD UNIQUE KEY `order_id` (`order_id`),
  ADD KEY `_deliverymanid` (`deliveryman_id`),
  ADD KEY `order id binding` (`order_id`);

--
-- Indexes for table `orders`
--
ALTER TABLE `orders`
  ADD PRIMARY KEY (`_id`),
  ADD KEY `_buyerid` (`buyer_id`),
  ADD KEY `_statusid` (`status_id`);

--
-- Indexes for table `order_items`
--
ALTER TABLE `order_items`
  ADD PRIMARY KEY (`order_id`,`product_id`,`seller_id`),
  ADD KEY `order_id` (`order_id`,`product_id`,`seller_id`),
  ADD KEY `bind product_id` (`product_id`),
  ADD KEY `bind seller_id` (`seller_id`);

--
-- Indexes for table `order_status`
--
ALTER TABLE `order_status`
  ADD PRIMARY KEY (`_id`),
  ADD UNIQUE KEY `status` (`status`);

--
-- Indexes for table `products`
--
ALTER TABLE `products`
  ADD PRIMARY KEY (`_id`),
  ADD KEY `availability_id` (`availability_id`),
  ADD KEY `category_id` (`category_id`),
  ADD KEY `seller product binding` (`seller_id`),
  ADD KEY `seller_id` (`seller_id`);

--
-- Indexes for table `product_spec`
--
ALTER TABLE `product_spec`
  ADD PRIMARY KEY (`_id`,`product_id`,`categories_spec_id`),
  ADD KEY `product_id` (`product_id`),
  ADD KEY `cate-spec_id` (`categories_spec_id`);

--
-- Indexes for table `rates`
--
ALTER TABLE `rates`
  ADD PRIMARY KEY (`_id`),
  ADD KEY `buyer_id` (`user_id`),
  ADD KEY `product_id` (`product_id`);

--
-- Indexes for table `sellers`
--
ALTER TABLE `sellers`
  ADD PRIMARY KEY (`user_id`),
  ADD UNIQUE KEY `user_id_2` (`user_id`),
  ADD KEY `_adminid` (`user_id`),
  ADD KEY `user_id` (`user_id`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`_id`),
  ADD KEY `user_type` (`user_type`),
  ADD KEY `user_status` (`user_status`);

--
-- Indexes for table `user_status`
--
ALTER TABLE `user_status`
  ADD PRIMARY KEY (`status_id`),
  ADD UNIQUE KEY `status_name` (`status_name`);

--
-- Indexes for table `user_type`
--
ALTER TABLE `user_type`
  ADD PRIMARY KEY (`type_id`),
  ADD UNIQUE KEY `name` (`name`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `availability_status`
--
ALTER TABLE `availability_status`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=3;
--
-- AUTO_INCREMENT for table `cart_items`
--
ALTER TABLE `cart_items`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `categories`
--
ALTER TABLE `categories`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `categories_spec`
--
ALTER TABLE `categories_spec`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `deliveryrequests`
--
ALTER TABLE `deliveryrequests`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `orders`
--
ALTER TABLE `orders`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `order_status`
--
ALTER TABLE `order_status`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=5;
--
-- AUTO_INCREMENT for table `products`
--
ALTER TABLE `products`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `product_spec`
--
ALTER TABLE `product_spec`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `rates`
--
ALTER TABLE `rates`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `users`
--
ALTER TABLE `users`
  MODIFY `_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=115;
--
-- AUTO_INCREMENT for table `user_status`
--
ALTER TABLE `user_status`
  MODIFY `status_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=3;
--
-- AUTO_INCREMENT for table `user_type`
--
ALTER TABLE `user_type`
  MODIFY `type_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=6;
--
-- Constraints for dumped tables
--

--
-- Constraints for table `accountants`
--
ALTER TABLE `accountants`
  ADD CONSTRAINT `Bind accountant id with user id` FOREIGN KEY (`user_id`) REFERENCES `users` (`_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `admins`
--
ALTER TABLE `admins`
  ADD CONSTRAINT `Bind admin id with user id` FOREIGN KEY (`user_id`) REFERENCES `users` (`_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `buyers`
--
ALTER TABLE `buyers`
  ADD CONSTRAINT `Bind user id` FOREIGN KEY (`user_id`) REFERENCES `users` (`_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `cart_items`
--
ALTER TABLE `cart_items`
  ADD CONSTRAINT `Bind cart item to user id` FOREIGN KEY (`user_id`) REFERENCES `buyers` (`user_id`),
  ADD CONSTRAINT `Bind product id to products` FOREIGN KEY (`product_id`) REFERENCES `products` (`_id`);

--
-- Constraints for table `categories_spec`
--
ALTER TABLE `categories_spec`
  ADD CONSTRAINT `Binding categories_specs with categories` FOREIGN KEY (`category_id`) REFERENCES `categories` (`_id`);

--
-- Constraints for table `deliverymen`
--
ALTER TABLE `deliverymen`
  ADD CONSTRAINT `Bind user_id` FOREIGN KEY (`user_id`) REFERENCES `users` (`_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `deliveryrequests`
--
ALTER TABLE `deliveryrequests`
  ADD CONSTRAINT `Bind order id` FOREIGN KEY (`order_id`) REFERENCES `orders` (`_id`),
  ADD CONSTRAINT `delivery man foreign key` FOREIGN KEY (`deliveryman_id`) REFERENCES `deliverymen` (`user_id`);

--
-- Constraints for table `orders`
--
ALTER TABLE `orders`
  ADD CONSTRAINT `buyer binding` FOREIGN KEY (`buyer_id`) REFERENCES `buyers` (`user_id`),
  ADD CONSTRAINT `order status binding` FOREIGN KEY (`status_id`) REFERENCES `order_status` (`_id`);

--
-- Constraints for table `order_items`
--
ALTER TABLE `order_items`
  ADD CONSTRAINT `bind order_id` FOREIGN KEY (`order_id`) REFERENCES `orders` (`_id`),
  ADD CONSTRAINT `bind product_id` FOREIGN KEY (`product_id`) REFERENCES `products` (`_id`),
  ADD CONSTRAINT `bind seller_id` FOREIGN KEY (`seller_id`) REFERENCES `sellers` (`user_id`);

--
-- Constraints for table `products`
--
ALTER TABLE `products`
  ADD CONSTRAINT `availability status binding` FOREIGN KEY (`availability_id`) REFERENCES `availability_status` (`_id`),
  ADD CONSTRAINT `categories product binding` FOREIGN KEY (`category_id`) REFERENCES `categories` (`_id`),
  ADD CONSTRAINT `selelr id ` FOREIGN KEY (`seller_id`) REFERENCES `sellers` (`user_id`);

--
-- Constraints for table `product_spec`
--
ALTER TABLE `product_spec`
  ADD CONSTRAINT `Binding  product_specs with cate_specs` FOREIGN KEY (`categories_spec_id`) REFERENCES `categories_spec` (`_id`),
  ADD CONSTRAINT `Binding  product_specs with product` FOREIGN KEY (`product_id`) REFERENCES `products` (`_id`);

--
-- Constraints for table `rates`
--
ALTER TABLE `rates`
  ADD CONSTRAINT `Binding product id` FOREIGN KEY (`product_id`) REFERENCES `products` (`_id`),
  ADD CONSTRAINT `Binding user_id` FOREIGN KEY (`user_id`) REFERENCES `buyers` (`user_id`);

--
-- Constraints for table `sellers`
--
ALTER TABLE `sellers`
  ADD CONSTRAINT `Binding user id ` FOREIGN KEY (`user_id`) REFERENCES `users` (`_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `users`
--
ALTER TABLE `users`
  ADD CONSTRAINT `bind user_status` FOREIGN KEY (`user_status`) REFERENCES `user_status` (`status_id`),
  ADD CONSTRAINT `bind user_type` FOREIGN KEY (`user_type`) REFERENCES `user_type` (`type_id`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
