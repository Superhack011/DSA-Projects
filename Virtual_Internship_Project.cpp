#include<iostream>
#include<cmath>
#include<algorithm>
#include<vector>
#include<unordered_map>
#include<ctime>
#include<queue>

using namespace std;

class Room {
public:
	int roomnumber;
	int capacity;
	bool isavl;

	Room() {
		this->roomnumber = -1;
		this->capacity = -1;
		this->isavl = false;
	}

	Room(int roomnumber, int capacity) {
		this->roomnumber = roomnumber;
		this->capacity = capacity;
		this->isavl = true;
	}
};

class Booking {
public:
	string hostname;
	int roomnumber;
	time_t startTime;
	time_t endTime;
	int chairs;

	Booking() {
		this->hostname = "";
		this->roomnumber = -1;
		this->startTime = 0;
		this->endTime = 0;
		this->chairs = -1;
	}

	Booking(string name, int roomnumber, time_t start, time_t end, int chairs) {
		this->hostname = name;
		this->roomnumber = roomnumber;
		this->startTime = start;
		this->endTime = end;
		this->chairs = chairs;
	}
};

//creating a min heap camparator, Small and best fitt room goes first!
struct Roomcamparator {
	bool operator()(const Room& a, const Room& b) {
		return a.capacity > b.capacity;
	}
};

class Scheduler {
private:
	priority_queue<Room, vector<Room>, Roomcamparator > roomallocation;
	priority_queue<pair<time_t, int>, vector<pair<time_t, int>>, greater<> > bookingQueue;
	unordered_map<int, Room> allRooms;
	vector<Booking> bookings;

	void roomDeallocate();

public:
	void addRoom(int roomnumber, int capacity);
	void bookRoom(string hostname, time_t start, time_t end, int chairs);
	void displayRooms();
	void displayBookings();
	void checkExit();
};

void Scheduler::addRoom(int roomnumber, int capacity) {
	if (allRooms.find(roomnumber) != allRooms.end()) {
		cout << "The Room is Already exists." << endl;
		cout << "Try Again !!" << endl;
		return;
	}
	Room room(roomnumber, capacity);
	roomallocation.push(room);
	allRooms[roomnumber] = room;

    cout<<"Room is successfully added."<<endl;
}
void Scheduler::bookRoom(string hostname, time_t start, time_t end, int chairs) {
	vector<Room> temp;
	Room selectRoom(-1, -1);

	while (!roomallocation.empty()) {
		Room room = roomallocation.top();
		roomallocation.pop();

		if (room.isavl && room.capacity >= chairs) {
			selectRoom = room;
			break;
		}

		temp.push_back(room);
	}

	// Push back unsuitable rooms
	for (const auto& room : temp) {
		roomallocation.push(room);
	}

	if (selectRoom.roomnumber == -1) {
		cout << "No suitable room is found!" << endl;
		return;
	}

	selectRoom.isavl = false;
	allRooms[selectRoom.roomnumber] = selectRoom;

	Booking booking(hostname, selectRoom.roomnumber, start, end, chairs);
	bookings.push_back(booking);
	bookingQueue.push({ end, selectRoom.roomnumber });
	cout << "Room " << selectRoom.roomnumber << " booked successfully by " << hostname << ".\n";

	return;
}


void Scheduler::displayRooms() {
	cout << "Room no.\tRoom Capacity\tRoom Availability" << endl;
	for (auto i : allRooms) {
		cout << i.first << "\t\t" << i.second.capacity << "\t\t" << i.second.isavl << endl;
	}
}

void Scheduler::displayBookings() {
	cout << "Room no.\tHost Name\tStart Time\tEnd Time\tChairs" << endl;
	for (auto i : bookings) {
		cout << i.roomnumber << "\t\t" << i.hostname << "\t" << ctime(&i.startTime) << "\t" << ctime(&i.endTime) << "\t\t" << i.chairs << endl;
	}
}

void Scheduler::roomDeallocate() {
	time_t cuurentTime = time(0);

	if (!bookingQueue.empty()) {
		pair<time_t, int> t = bookingQueue.top();
		if (t.first <= cuurentTime) {
			bookingQueue.pop();

			Room& room = allRooms[t.second];
			allRooms[t.second] = room;
			room.isavl = true;
			
			//Re-build the heap
			priority_queue<Room, vector<Room>, Roomcamparator> newheap;
			for (auto& i : allRooms) {
				newheap.push(i.second);
			}

			roomallocation = move(newheap);
		}
	}
}

void Scheduler::checkExit() {
	roomDeallocate();
}

void menu() {
	cout << "\t Menu : " << endl;
	cout << " 1. Show Menu\n 2. Book a Room \n 3. Add a Room \n 4. Show Rooms \n 5. Show Bookings\n 6. Exit" << endl;

	for (int i = 0; i < 20; i++) cout << "_";

	cout << endl;
}

void choice(int x, Scheduler& schedule) {
	switch (x) {
	case 1: {
		menu();
		break;
	}
	case 2: {
		string name; cout << "Enter the host name : "; cin >> name;
		time_t startTime, endTime; cout << "\nEnter the Start Time : "; cin >> startTime;
		cout << "\nEnter the end Time : "; cin >> endTime;
		int chairs;
		cout << "\nEnter the number of chairs :"; cin >> chairs;

		schedule.bookRoom(name, startTime, endTime, chairs);
		break;
	}
	case 3: {
		int roomnumber; cout << "Enter the room number : "; cin >> roomnumber;
		int capacity; cout << "Enter the capacity for the Room : "; cin >> capacity;

		schedule.addRoom(roomnumber, capacity);
		break;
	}
	case 4: {
		cout << "All the Rooms are listed following : \n";
		schedule.displayRooms();
		break;
	}
	case 5: {
		cout << "All the Current bookings are listed following : \n";
		schedule.displayBookings();
		break;
	}
	default:
		cout << "Please enter a valid option. "<<endl; 
		break;
	}
}

int main() {
	Scheduler schedule;

	cout << "Welcome !!" << endl;
	menu();

	int x;
	while (true) {
		cout << "Enter the choice : "; cin >> x;
		if (x == 6) {
			cout << "Thanks for Giving an Opportunity to serve you."; break;
		}
		choice(x, schedule);
	}

	return 0;
}
