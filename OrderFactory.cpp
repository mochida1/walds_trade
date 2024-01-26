#include "OrderFactory.hpp"
#include <random>
#include <ctime>

//modify these to configure default class constructor;
//do not commit changes into those. They're the closes to the examples provided.
uint32_t gMaxSecIds = 5;
uint32_t gBuyRatio = 50;
uint32_t gMinQty = 100;
uint32_t gMaxQty = 3000;
uint32_t gMaxUsers = 10;
uint32_t gMaxCompanies = 3;
uint8_t gQtyResolution = 2;
uint32_t gIdIndex = 1;

//generates random number no matter the time
static int rngeezus(int min, int max)
{
	int delta;
	int ret;

	srand(time(0) + rand());
	if (min >= max)
		return 0;

	delta = max - min;
	ret = std::rand()%delta;
	ret += min;
	return ret;
}

//default constructor
OrderFactory::OrderFactory(void){
	this->_maxSecIds = gMaxSecIds;
	this->_buyRatio = gBuyRatio;
	this->_minQty = gMinQty;
	this->_maxQty = gMaxQty;
	this->_maxUsers = gMaxUsers;
	this->_maxCompanies = gMaxCompanies;
	this->_qtyResolution = gQtyResolution;
	this->_idIndex = gIdIndex;
	return ;
}

OrderFactory::~OrderFactory(void){
	return ;
}

void OrderFactory::setMaxSecIds(uint32_t maxSecIds){
	this->_maxSecIds = maxSecIds;
	return ;
}

void OrderFactory::setBuyRatio(uint32_t buyRatio){
	this->_buyRatio = buyRatio;
	return ;
}

void OrderFactory::setMinQty(uint32_t minQty){
	this->_minQty = minQty;
	return ;
}

void OrderFactory::setMaxQty(uint32_t maxQty){
	this->_maxQty = maxQty;
	return ;
}
void OrderFactory::setMaxUsers(uint32_t maxUsers){
	this->_maxUsers = maxUsers;
	return ;
}

void OrderFactory::setMaxCompanies(uint32_t maxCompanies){
	this->_maxCompanies = maxCompanies;
	return ;
}

void OrderFactory::setQtyResolution(uint8_t resolution){
	this->_qtyResolution = resolution;
	return ;
}

void OrderFactory::setIdIndex(uint32_t index){
	this->_idIndex = index;
	return ;
}

void OrderFactory::printSettings(void) const{
	std::cout << "MaxSecIds: [" << this->_maxSecIds;
	std::cout << "] | BuyRatio: [" << this->_buyRatio;
	std::cout << "] | MinQty: [" << this->_minQty;
	std::cout << "] | MaxQty: [" << this->_maxQty;
	std::cout << "] | MaxUsers: [" << this->_maxUsers;
	std::cout << "] | MaxCompanies: [" << this->_maxCompanies;
	std::cout << "] | QtyResolution: [" << (int)(this->_qtyResolution) << "]" << std::endl;
	return ;
}

void OrderFactory::printOrder(const Order &order){
	std::cout << "orderId:[" << order.orderId();
	std::cout << "] | securityId:[" << order.securityId();
	std::cout << "] | side:[" << order.side();
	std::cout << "] | user:[" << order.user();
	std::cout << "] | company:[" << order.company();
	std::cout << "] | qty:[" << order.qty() << "]" << std::endl;
}

void OrderFactory::printOrderContainer(std::vector<Order> &container){
	uint32_t size = 0;
	for (const auto &it : container){
		OrderFactory::printOrder(it);
		size++;
	}
	std::cout << "Listed " << size << " entries." << std::endl;
}

void OrderFactory::printOrderContainer(std::unordered_map<std::string, Order> &container){
	for (const auto &it : container){
		OrderFactory::printOrder(it.second);
	}
}

void OrderFactory::printOrderContainer(std::deque<Order> &container){
	for (const auto &it : container){
		OrderFactory::printOrder(it);
	}
}

Order OrderFactory::singleOrderGenerator(void){
	uint32_t leftover;
	std::string orderId = "OrdId" + std::to_string(this->_idIndex);
	std::string secId = "SecId" + std::to_string(rngeezus(1, this->_maxSecIds+1));
	std::string side = rngeezus(0, 99) < this->_buyRatio ? "Buy" : "Sell";
	uint32_t qty = rngeezus(this->_minQty, this->_maxQty);
	leftover = qty % (uint32_t)(pow(10, this->_qtyResolution));
	qty -= leftover;
	std::string user = "User" + std::to_string(rngeezus(1, this->_maxUsers+1));
	std::string company = "Company" + std::to_string(rngeezus(1, this->_maxCompanies+1));
	Order ret(orderId, secId, side, qty, user, company);
	this->_idIndex++;
	return ret;
}

// adds any ammount of random orders
void OrderFactory::batchOrderGenerator(uint32_t ammount, OrderCache &cache){
	for (uint32_t i = 0; i < ammount; i++)
	{
		cache.addOrder(this->singleOrderGenerator());
	}
	return ;
}

// Order{"OrdId3", "SecId1", "Buy", 300, "User13", "Company2"}
std::string OrderFactory::createOrderAsString(void){
	Order orderToConvert(this->singleOrderGenerator());
	std::string ret = orderToConvert.orderId() + " " + orderToConvert.securityId() + " " + orderToConvert.side() + " ";
	ret += std::to_string(orderToConvert.qty()) + " " + orderToConvert.user() + " " + orderToConvert.company() + "\n";
	return ret;
}


std::string OrderFactory::createBenchmarkFile(uint32_t totalEntries, std::string fileName){
	OrderFactory OF;
	std::ofstream outFile(fileName);
	if (outFile.is_open()){
		for (uint32_t i = 1; i <= totalEntries; i++){
			outFile << this->createOrderAsString();
		}
		outFile.close();
	}
	else{
		std::cerr << "Error: could not create file!" << std::endl;
		return "";
	}
	return fileName;
}

uint32_t OrderFactory::batchOrderFromFile(std::string file, OrderCache &cache){
	std::ifstream fileFS(file);
	uint32_t linesRead = 0;
	std::vector<std::string> tokens;
	tokens.reserve(6);
	if (fileFS.is_open()){
		std::string line;
		while (std::getline(fileFS, line)){
			std::istringstream iss(line);
			std::string token;
			while (iss >> token){
				tokens.push_back(token);
			}
			cache.addOrder(Order{tokens[0], tokens[1], tokens[2], (unsigned int)(std::stoul(tokens[3])), tokens[4], tokens[5]});
			tokens.clear();
		}
		fileFS.close();
	}
	else{
		std::cerr << "ERROR: Could no open file: " << file << std::endl;
	}
	return linesRead;
}