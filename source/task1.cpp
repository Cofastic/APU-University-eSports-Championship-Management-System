#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

// Player structure to hold player information
struct Player {
    std::string playerID;
    std::string playerName;
    std::string university;
    std::string gameSpecialty;
    int skillRating;
    std::string registrationType; // "early-bird", "regular", "wildcard"
    std::string status; // "registered", "checked-in", "withdrawn"
    int priority; // Lower number = higher priority
    
    Player() : skillRating(0), priority(0) {}
    
    Player(std::string id, std::string name, std::string uni, std::string game, 
           int rating, std::string regType, std::string stat) 
        : playerID(id), playerName(name), university(uni), gameSpecialty(game),
          skillRating(rating), registrationType(regType), status(stat) {
        setPriority();
    }
    
    // Set priority based on registration type
    void setPriority() {
        if (registrationType == "early-bird") {
            priority = 1; // Highest priority
        } else if (registrationType == "wildcard") {
            priority = 2; // Medium priority
        } else {
            priority = 3; // Regular priority
        }
    }
};

// Priority Queue Node for player management
struct PriorityQueueNode {
    Player player;
    PriorityQueueNode* next;
    
    PriorityQueueNode(Player p) : player(p), next(nullptr) {}
};

// Priority Queue implementation for tournament registration
class TournamentRegistrationQueue {
private:
    PriorityQueueNode* front;
    int queueSize;
    
public:
    TournamentRegistrationQueue() {
        front = nullptr;
        queueSize = 0;
    }
    
    ~TournamentRegistrationQueue() {
        clearQueue();
    }
    
    // Enqueue player based on priority (lower priority number = higher priority)
    void enqueue(Player player) {
        PriorityQueueNode* newNode = new PriorityQueueNode(player);
        
        // If queue is empty or new player has higher priority than front
        if (front == nullptr || player.priority < front->player.priority) {
            newNode->next = front;
            front = newNode;
        } else {
            // Find the correct position to insert
            PriorityQueueNode* current = front;
            while (current->next != nullptr && 
                   current->next->player.priority <= player.priority) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
        queueSize++;
        std::cout << "Player " << player.playerName << " (" << player.registrationType 
                  << ") added to registration queue with priority " << player.priority << std::endl;
    }
    
    // Dequeue highest priority player
    Player dequeue() {
        if (isEmpty()) {
            std::cout << "Registration queue is empty!" << std::endl;
            return Player();
        }
        
        PriorityQueueNode* temp = front;
        Player player = front->player;
        front = front->next;
        delete temp;
        queueSize--;
        
        return player;
    }
    
    // Peek at highest priority player without removing
    Player peek() {
        if (isEmpty()) {
            std::cout << "Registration queue is empty!" << std::endl;
            return Player();
        }
        return front->player;
    }
    
    // Check if queue is empty
    bool isEmpty() {
        return front == nullptr;
    }
    
    // Get queue size
    int size() {
        return queueSize;
    }
    
    // Load players from CSV file - Enhanced with error handling
    void loadPlayersFromCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not open file " << filename << std::endl;
            std::cout << "Please ensure the file exists and is accessible." << std::endl;
            return;
        }
        
        std::string line;
        if (!std::getline(file, line)) {
            std::cout << "Error: File " << filename << " is empty or corrupted." << std::endl;
            file.close();
            return;
        }
        
        int lineNumber = 1;
        int loadedCount = 0;
        int errorCount = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            if (line.empty()) {
                continue; // Skip empty lines
            }
            
            try {
                std::stringstream ss(line);
                std::string playerID, playerName, university, gameSpecialty, 
                           skillRatingStr, registrationType, status;
                
                std::getline(ss, playerID, ',');
                std::getline(ss, playerName, ',');
                std::getline(ss, university, ',');
                std::getline(ss, gameSpecialty, ',');
                std::getline(ss, skillRatingStr, ',');
                std::getline(ss, registrationType, ',');
                std::getline(ss, status, ',');
                
                // Validate required fields
                if (playerID.empty() || playerName.empty() || skillRatingStr.empty()) {
                    std::cout << "Warning: Line " << lineNumber << " has missing required fields, skipping." << std::endl;
                    errorCount++;
                    continue;
                }
                
                // Skip additional columns if they exist (for extended datasets)
                std::string temp;
                while (std::getline(ss, temp, ',')) {
                    // Skip remaining columns for compatibility
                }
                
                int skillRating;
                try {
                    skillRating = std::stoi(skillRatingStr);
                    if (skillRating < 0 || skillRating > 1000) {
                        std::cout << "Warning: Line " << lineNumber << " has invalid skill rating (" 
                                  << skillRating << "), using default value 500." << std::endl;
                        skillRating = 500;
                    }
                } catch (const std::exception& e) {
                    std::cout << "Warning: Line " << lineNumber << " has invalid skill rating format, using default value 500." << std::endl;
                    skillRating = 500;
                }
                
                // Validate registration type
                if (registrationType != "early-bird" && registrationType != "regular" && registrationType != "wildcard") {
                    std::cout << "Warning: Line " << lineNumber << " has invalid registration type (" 
                              << registrationType << "), using 'regular'." << std::endl;
                    registrationType = "regular";
                }
                
                // Validate status
                if (status != "registered" && status != "checked-in" && status != "withdrawn") {
                    status = "registered"; // Default status
                }
                
                // Check for duplicate player IDs
                if (isPlayerAlreadyInQueue(playerID)) {
                    std::cout << "Warning: Player ID " << playerID << " already exists, skipping duplicate." << std::endl;
                    errorCount++;
                    continue;
                }
                
                Player player(playerID, playerName, university, gameSpecialty, 
                             skillRating, registrationType, status);
                
                enqueue(player);
                loadedCount++;
                
            } catch (const std::exception& e) {
                std::cout << "Error processing line " << lineNumber << ": " << e.what() << std::endl;
                errorCount++;
            }
        }
        
        file.close();
        
        std::cout << "\n=== LOADING SUMMARY ===" << std::endl;
        std::cout << "Successfully loaded: " << loadedCount << " players" << std::endl;
        if (errorCount > 0) {
            std::cout << "Errors/Warnings: " << errorCount << " lines" << std::endl;
        }
        std::cout << "Total queue size: " << queueSize << std::endl;
        std::cout << "=======================" << std::endl;
    }
    
    // Export checked-in players for other tasks (Task 1 - Match Scheduling)
    void exportCheckedInPlayersForMatching(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not create file " << filename << std::endl;
            return;
        }
        
        file << "PlayerID,PlayerName,University,GameSpecialty,SkillRating,Status,CheckInOrder" << std::endl;
        
        PriorityQueueNode* current = front;
        int checkInOrder = 1;
        
        while (current != nullptr) {
            Player p = current->player;
            if (p.status == "checked-in") {
                file << p.playerID << "," << p.playerName << "," << p.university << ","
                     << p.gameSpecialty << "," << p.skillRating << ",checked-in," 
                     << checkInOrder << std::endl;
                checkInOrder++;
            }
            current = current->next;
        }
        
        file.close();
        std::cout << "Checked-in players exported to " << filename << " for match scheduling" << std::endl;
    }
    
    // Load from shared minimal datasets
    void loadFromSharedDatasets() {
        std::cout << "\n=== LOADING FROM SHARED DATASETS ===" << std::endl;
        
        // Load primary player data
        loadPlayersFromCSV("players.csv");
        
        std::cout << "\nDataset usage by tasks:" << std::endl;
        std::cout << "1. players.csv - Used by ALL tasks (main data)" << std::endl;
        std::cout << "2. matches.csv - Used by Task 1 & Task 4" << std::endl;
        std::cout << "3. spectators.csv - Used ONLY by Task 3" << std::endl;
        std::cout << "=====================================" << std::endl;
    }
    
    // Auto-save queue to CSV after any changes
    void autoSaveToCSV() {
        savePlayersToCSV("C:\\APU\\GitHub\\C++\\APU-University-eSports-Championship-Management-System\\source\\data\\players.csv");
    }
    
    // Save current queue to CSV file
    void savePlayersToCSV(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not create file " << filename << std::endl;
            return;
        }
        
        file << "PlayerID,PlayerName,University,GameSpecialty,SkillRating,RegistrationType,Status" << std::endl;
        
        PriorityQueueNode* current = front;
        while (current != nullptr) {
            Player p = current->player;
            file << p.playerID << "," << p.playerName << "," << p.university << ","
                 << p.gameSpecialty << "," << p.skillRating << "," 
                 << p.registrationType << "," << p.status << std::endl;
            current = current->next;
        }
        
        file.close();
        std::cout << "Queue saved to " << filename << std::endl;
    }
    

    
    // Process check-in with player selection
    void checkInPlayer() {
        if (isEmpty()) {
            std::cout << "No players in registration queue!" << std::endl;
            return;
        }
        
        // Show players available for check-in (only registered players)
        std::cout << "\n========== AVAILABLE PLAYERS FOR CHECK-IN ==========" << std::endl;
        std::cout << "Players shown in priority order (early-bird > wildcard > regular)" << std::endl;
        std::cout << "====================================================" << std::endl;
        
        PriorityQueueNode* current = front;
        int availableCount = 0;
        int displayIndex = 1;
        
        // First pass: count and display available players
        while (current != nullptr) {
            if (current->player.status == "registered") {
                std::cout << displayIndex << ". " << current->player.playerName << std::endl;
                std::cout << "   ID: " << current->player.playerID 
                          << " | University: " << current->player.university << std::endl;
                std::cout << "   Game: " << current->player.gameSpecialty 
                          << " | Priority: " << current->player.priority 
                          << " (" << current->player.registrationType << ")" << std::endl;
                std::cout << "   ---" << std::endl;
                availableCount++;
                displayIndex++;
            }
            current = current->next;
        }
        
        if (availableCount == 0) {
            std::cout << "No players available for check-in (all players already checked-in or withdrawn)." << std::endl;
            return;
        }
        
        std::cout << "====================================================" << std::endl;
        
        // Get user selection
        int choice;
        while (true) {
            std::cout << "Select player to check in (1-" << availableCount << ") or 0 to cancel: ";
            std::cin >> choice;
            
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Error: Please enter a valid number." << std::endl;
                continue;
            }
            
            if (choice == 0) {
                std::cout << "Check-in cancelled." << std::endl;
                return;
            }
            
            if (choice >= 1 && choice <= availableCount) {
                break;
            }
            
            std::cout << "Error: Please select a number between 1 and " << availableCount << "." << std::endl;
        }
        
        // Find and check in the selected player
        current = front;
        int currentIndex = 1;
        
        while (current != nullptr) {
            if (current->player.status == "registered") {
                if (currentIndex == choice) {
                    current->player.status = "checked-in";
                    
                    std::cout << "\n=== PLAYER CHECK-IN SUCCESSFUL ===" << std::endl;
                    std::cout << "Player ID: " << current->player.playerID << std::endl;
                    std::cout << "Name: " << current->player.playerName << std::endl;
                    std::cout << "University: " << current->player.university << std::endl;
                    std::cout << "Game Specialty: " << current->player.gameSpecialty << std::endl;
                    std::cout << "Registration Type: " << current->player.registrationType << std::endl;
                    std::cout << "Status: " << current->player.status << std::endl;
                    std::cout << "=================================" << std::endl;
                    
                    // Auto-save changes to CSV
                    autoSaveToCSV();
                    std::cout << "Data automatically saved to CSV file." << std::endl;
                    return;
                }
                currentIndex++;
            }
            current = current->next;
        }
    }
    
    // Handle player withdrawal with enhanced error handling
    void handleWithdrawal(const std::string& playerID) {
        if (isEmpty()) {
            std::cout << "Error: No players in registration queue!" << std::endl;
            return;
        }
        
        if (playerID.empty()) {
            std::cout << "Error: Player ID cannot be empty!" << std::endl;
            return;
        }
        
        // Search for player and update status to withdrawn
        PriorityQueueNode* current = front;
        bool playerFound = false;
        
        while (current != nullptr) {
            if (current->player.playerID == playerID) {
                current->player.status = "withdrawn";
                std::cout << "\n=== WITHDRAWAL SUCCESSFUL ===" << std::endl;
                std::cout << "Player " << current->player.playerName << " (ID: " << playerID 
                          << ") has been withdrawn from the tournament." << std::endl;
                std::cout << "============================" << std::endl;
                playerFound = true;
                break;
            }
            current = current->next;
        }
        
        if (playerFound) {
            // Auto-save changes to CSV
            autoSaveToCSV();
            std::cout << "Data automatically saved to CSV file." << std::endl;
        } else {
            std::cout << "\nError: Player with ID '" << playerID << "' not found in registration queue." << std::endl;
            std::cout << "Please check the Player ID and try again." << std::endl;
        }
    }
    
    // Add wildcard entry with manual player entry
    void addWildcardEntry() {
        std::cout << "\n=== ADD WILDCARD ENTRY ===" << std::endl;
        std::cout << "Enter wildcard player details:" << std::endl;
        std::cout << "==========================" << std::endl;
        
        std::string playerID, playerName, university, gameSpecialty;
        int skillRating, gameChoice;
        
        // Get player details with input validation
        while (true) {
            std::cout << "Enter Player ID (must start with 'P'): ";
            std::cin >> playerID;
            
            if (playerID.empty()) {
                std::cout << "Error: Player ID cannot be empty. Please try again." << std::endl;
                continue;
            }
            
            if (playerID.length() < 2 || playerID[0] != 'P') {
                std::cout << "Error: Player ID must start with 'P' (e.g., P001, P100). Please try again." << std::endl;
                continue;
            }
            
            // Check for duplicate player IDs
            if (isPlayerAlreadyInQueue(playerID)) {
                std::cout << "Error: Player ID '" << playerID << "' already exists in the queue!" << std::endl;
                std::cout << "Please use a different Player ID." << std::endl;
                continue;
            }
            
            break; // Valid unique player ID
        }
        
        std::cin.ignore(); // Clear input buffer
        
        while (true) {
            std::cout << "Enter Player Name: ";
            std::getline(std::cin, playerName);
            
            if (!playerName.empty() && playerName.find_first_not_of(" \t\n\r") != std::string::npos) {
                break;
            }
            std::cout << "Error: Player name cannot be empty or contain only spaces. Please try again." << std::endl;
        }
        
        while (true) {
            std::cout << "Enter University: ";
            std::getline(std::cin, university);
            
            if (!university.empty() && university.find_first_not_of(" \t\n\r") != std::string::npos) {
                break;
            }
            std::cout << "Error: University cannot be empty or contain only spaces. Please try again." << std::endl;
        }
        
        // Game specialty selection menu
        while (true) {
            std::cout << "\nSelect Game Specialty:" << std::endl;
            std::cout << "1. League of Legends" << std::endl;
            std::cout << "2. Valorant" << std::endl;
            std::cout << "3. Counter-Strike 2" << std::endl;
            std::cout << "4. Overwatch 2" << std::endl;
            std::cout << "Enter your choice (1-4): ";
            
            std::cin >> gameChoice;
            
            if (std::cin.fail()) {
                std::cin.clear(); // Clear error flag
                std::cin.ignore(10000, '\n'); // Clear input buffer
                std::cout << "Error: Please enter a valid number (1-4)." << std::endl;
                continue;
            }
            
            switch (gameChoice) {
                case 1:
                    gameSpecialty = "League of Legends";
                    break;
                case 2:
                    gameSpecialty = "Valorant";
                    break;
                case 3:
                    gameSpecialty = "Counter-Strike 2";
                    break;
                case 4:
                    gameSpecialty = "Overwatch 2";
                    break;
                default:
                    std::cout << "Error: Invalid choice. Please select 1-4." << std::endl;
                    continue;
            }
            break; // Valid game selection
        }
        
        while (true) {
            std::cout << "Enter Skill Rating (0-1000): ";
            std::cin >> skillRating;
            
            if (std::cin.fail()) {
                std::cin.clear(); // Clear error flag
                std::cin.ignore(10000, '\n'); // Clear input buffer
                std::cout << "Error: Please enter a valid number." << std::endl;
                continue;
            }
            
            if (skillRating >= 0 && skillRating <= 1000) {
                break;
            }
            
            std::cout << "Error: Skill rating must be between 0 and 1000. Please try again." << std::endl;
        }
        
        // Create and add wildcard player
        Player wildcardPlayer(playerID, playerName, university, gameSpecialty, 
                            skillRating, "wildcard", "registered");
        
        std::cout << "\n=== WILDCARD ENTRY CONFIRMATION ===" << std::endl;
        std::cout << "Player ID: " << playerID << std::endl;
        std::cout << "Name: " << playerName << std::endl;
        std::cout << "University: " << university << std::endl;
        std::cout << "Game: " << gameSpecialty << std::endl;
        std::cout << "Skill Rating: " << skillRating << std::endl;
        std::cout << "Registration Type: wildcard (Priority: 2)" << std::endl;
        std::cout << "===================================" << std::endl;
        
        char confirm;
        std::cout << "Confirm adding this wildcard player? (y/n): ";
        std::cin >> confirm;
        
        if (confirm == 'y' || confirm == 'Y') {
            enqueue(wildcardPlayer);
            std::cout << "\nWildcard player " << playerName << " successfully added to the registration queue!" << std::endl;
            
            // Auto-save changes to CSV
            autoSaveToCSV();
            std::cout << "Data automatically saved to CSV file." << std::endl;
        } else {
            std::cout << "\nWildcard entry cancelled." << std::endl;
        }
    }
    
    // Check if player already exists in queue
    bool isPlayerAlreadyInQueue(const std::string& playerID) {
        PriorityQueueNode* current = front;
        while (current != nullptr) {
            if (current->player.playerID == playerID) {
                return true;
            }
            current = current->next;
        }
        return false;
    }
    
    // Display entire registration queue
    void displayRegistrationQueue() {
        if (isEmpty()) {
            std::cout << "Registration queue is empty!" << std::endl;
            return;
        }
        
        std::cout << "\n========== TOURNAMENT REGISTRATION QUEUE ==========" << std::endl;
        std::cout << "Total Players in Queue: " << queueSize << std::endl;
        std::cout << "====================================================" << std::endl;
        
        PriorityQueueNode* current = front;
        int position = 1;
        
        while (current != nullptr) {
            Player p = current->player;
            std::cout << "Position " << position << ":" << std::endl;
            std::cout << "  Player ID: " << p.playerID << std::endl;
            std::cout << "  Name: " << p.playerName << std::endl;
            std::cout << "  University: " << p.university << std::endl;
            std::cout << "  Game: " << p.gameSpecialty << std::endl;
            std::cout << "  Skill Rating: " << p.skillRating << std::endl;
            std::cout << "  Registration Type: " << p.registrationType 
                      << " (Priority: " << p.priority << ")" << std::endl;
            std::cout << "  Status: " << p.status << std::endl;
            std::cout << "  ---" << std::endl;
            
            current = current->next;
            position++;
        }
        std::cout << "====================================================" << std::endl;
    }
    
    // Display players by status
    void displayPlayersByStatus() {
        if (isEmpty()) {
            std::cout << "Registration queue is empty!" << std::endl;
            return;
        }
        
        std::cout << "\n========== PLAYERS BY STATUS ==========" << std::endl;
        
        // Display Registered Players
        std::cout << "\nREGISTERED PLAYERS:" << std::endl;
        std::cout << "-------------------" << std::endl;
        PriorityQueueNode* current = front;
        int registeredCount = 0;
        while (current != nullptr) {
            if (current->player.status == "registered") {
                registeredCount++;
                std::cout << registeredCount << ". " << current->player.playerName 
                          << " (ID: " << current->player.playerID 
                          << ", " << current->player.registrationType << ")" << std::endl;
            }
            current = current->next;
        }
        if (registeredCount == 0) std::cout << "No registered players." << std::endl;
        
        // Display Checked-In Players
        std::cout << "\nCHECKED-IN PLAYERS:" << std::endl;
        std::cout << "-------------------" << std::endl;
        current = front;
        int checkedInCount = 0;
        while (current != nullptr) {
            if (current->player.status == "checked-in") {
                checkedInCount++;
                std::cout << checkedInCount << ". " << current->player.playerName 
                          << " (ID: " << current->player.playerID 
                          << ", " << current->player.registrationType << ")" << std::endl;
            }
            current = current->next;
        }
        if (checkedInCount == 0) std::cout << "No checked-in players." << std::endl;
        
        // Display Withdrawn Players
        std::cout << "\nWITHDRAWN PLAYERS:" << std::endl;
        std::cout << "------------------" << std::endl;
        current = front;
        int withdrawnCount = 0;
        while (current != nullptr) {
            if (current->player.status == "withdrawn") {
                withdrawnCount++;
                std::cout << withdrawnCount << ". " << current->player.playerName 
                          << " (ID: " << current->player.playerID 
                          << ", " << current->player.registrationType << ")" << std::endl;
            }
            current = current->next;
        }
        if (withdrawnCount == 0) std::cout << "No withdrawn players." << std::endl;
        
        std::cout << "\n=======================================" << std::endl;
    }
    
    // Display queue summary
    void displayQueueSummary() {
        if (isEmpty()) {
            std::cout << "Registration queue is empty!" << std::endl;
            return;
        }
        
        std::cout << "\n========== QUEUE SUMMARY ==========" << std::endl;
        std::cout << "Total Players: " << queueSize << std::endl;
        
        int registered = 0, checkedIn = 0, withdrawn = 0;
        int earlyBird = 0, wildcard = 0, regular = 0;
        
        PriorityQueueNode* current = front;
        while (current != nullptr) {
            // Count by status
            if (current->player.status == "registered") registered++;
            else if (current->player.status == "checked-in") checkedIn++;
            else if (current->player.status == "withdrawn") withdrawn++;
            
            // Count by registration type
            if (current->player.registrationType == "early-bird") earlyBird++;
            else if (current->player.registrationType == "wildcard") wildcard++;
            else regular++;
            
            current = current->next;
        }
        
        std::cout << "\nBy Status:" << std::endl;
        std::cout << "  Registered: " << registered << std::endl;
        std::cout << "  Checked-In: " << checkedIn << std::endl;
        std::cout << "  Withdrawn: " << withdrawn << std::endl;
        
        std::cout << "\nBy Registration Type:" << std::endl;
        std::cout << "  Early Bird: " << earlyBird << std::endl;
        std::cout << "  Wildcard: " << wildcard << std::endl;
        std::cout << "  Regular: " << regular << std::endl;
        
        std::cout << "===================================" << std::endl;
    }
    
    // View check-in status
    void viewCheckInStatus() {
        if (isEmpty()) {
            std::cout << "Registration queue is empty!" << std::endl;
            return;
        }
        
        std::cout << "\n========== CHECK-IN STATUS ==========" << std::endl;
        
        int totalPlayers = queueSize;
        int checkedIn = 0;
        int awaitingCheckIn = 0;
        
        PriorityQueueNode* current = front;
        while (current != nullptr) {
            if (current->player.status == "checked-in") {
                checkedIn++;
            } else if (current->player.status == "registered") {
                awaitingCheckIn++;
            }
            current = current->next;
        }
        
        std::cout << "Total Players: " << totalPlayers << std::endl;
        std::cout << "Checked In: " << checkedIn << std::endl;
        std::cout << "Awaiting Check-In: " << awaitingCheckIn << std::endl;
        
        if (awaitingCheckIn > 0) {
            std::cout << "\nNext in line for check-in:" << std::endl;
            current = front;
            int count = 0;
            while (current != nullptr && count < 3) {
                if (current->player.status == "registered") {
                    count++;
                    std::cout << count << ". " << current->player.playerName 
                              << " (" << current->player.registrationType << ")" << std::endl;
                }
                current = current->next;
            }
        }
        
        std::cout << "=====================================" << std::endl;
    }
    
    // Display players grouped by priority level
    void displayPlayersByPriority() {
        if (isEmpty()) {
            std::cout << "Registration queue is empty!" << std::endl;
            return;
        }
        
        std::cout << "\n========== PLAYERS BY PRIORITY LEVEL ==========" << std::endl;
        
        // Display Early Bird Players (Priority 1)
        std::cout << "\nEARLY BIRD PLAYERS (Priority 1):" << std::endl;
        std::cout << "---------------------------------" << std::endl;
        PriorityQueueNode* current = front;
        int earlyBirdCount = 0;
        while (current != nullptr) {
            if (current->player.priority == 1) {
                earlyBirdCount++;
                std::cout << earlyBirdCount << ". " << current->player.playerName 
                          << " (ID: " << current->player.playerID 
                          << ", Status: " << current->player.status << ")" << std::endl;
            }
            current = current->next;
        }
        if (earlyBirdCount == 0) std::cout << "No early bird players." << std::endl;
        
        // Display Wildcard Players (Priority 2)
        std::cout << "\nWILDCARD PLAYERS (Priority 2):" << std::endl;
        std::cout << "-------------------------------" << std::endl;
        current = front;
        int wildcardCount = 0;
        while (current != nullptr) {
            if (current->player.priority == 2) {
                wildcardCount++;
                std::cout << wildcardCount << ". " << current->player.playerName 
                          << " (ID: " << current->player.playerID 
                          << ", Status: " << current->player.status << ")" << std::endl;
            }
            current = current->next;
        }
        if (wildcardCount == 0) std::cout << "No wildcard players." << std::endl;
        
        // Display Regular Players (Priority 3)
        std::cout << "\nREGULAR PLAYERS (Priority 3):" << std::endl;
        std::cout << "------------------------------" << std::endl;
        current = front;
        int regularCount = 0;
        while (current != nullptr) {
            if (current->player.priority == 3) {
                regularCount++;
                std::cout << regularCount << ". " << current->player.playerName 
                          << " (ID: " << current->player.playerID 
                          << ", Status: " << current->player.status << ")" << std::endl;
            }
            current = current->next;
        }
        if (regularCount == 0) std::cout << "No regular players." << std::endl;
        
        std::cout << "\nSUMMARY:" << std::endl;
        std::cout << "Early Bird: " << earlyBirdCount << " | Wildcard: " << wildcardCount 
                  << " | Regular: " << regularCount << std::endl;
        std::cout << "===============================================" << std::endl;
    }

    
    // Generate registration statistics report
    void generateRegistrationReport() {
        if (isEmpty()) {
            std::cout << "No registration data available!" << std::endl;
            return;
        }
        
        int totalPlayers = queueSize;
        int earlyBird = 0, wildcard = 0, regular = 0;
        int registered = 0, checkedIn = 0, withdrawn = 0;
        
        PriorityQueueNode* current = front;
        while (current != nullptr) {
            // Count by registration type
            if (current->player.registrationType == "early-bird") earlyBird++;
            else if (current->player.registrationType == "wildcard") wildcard++;
            else regular++;
            
            // Count by status
            if (current->player.status == "registered") registered++;
            else if (current->player.status == "checked-in") checkedIn++;
            else if (current->player.status == "withdrawn") withdrawn++;
            
            current = current->next;
        }
        
        std::cout << "\n========== TOURNAMENT REGISTRATION REPORT ==========" << std::endl;
        std::cout << "Total Players in System: " << totalPlayers << std::endl;
        std::cout << "\nRegistration Types:" << std::endl;
        std::cout << "  Early Bird: " << earlyBird << " (" 
                  << (earlyBird * 100.0 / totalPlayers) << "%)" << std::endl;
        std::cout << "  Wildcard: " << wildcard << " (" 
                  << (wildcard * 100.0 / totalPlayers) << "%)" << std::endl;
        std::cout << "  Regular: " << regular << " (" 
                  << (regular * 100.0 / totalPlayers) << "%)" << std::endl;
        std::cout << "\nPlayer Status:" << std::endl;
        std::cout << "  Registered: " << registered << std::endl;
        std::cout << "  Checked In: " << checkedIn << std::endl;
        std::cout << "  Withdrawn: " << withdrawn << std::endl;
        std::cout << "===================================================" << std::endl;
    }
    
    // Clear screen function
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    // Display main menu with organized categories
    void displayMainMenu() {
        clearScreen();
        std::cout << "\n========== TOURNAMENT REGISTRATION SYSTEM ==========" << std::endl;
        std::cout << "                    MAIN MENU" << std::endl;
        std::cout << "====================================================" << std::endl;
        
        std::cout << "\nREGISTRATION & REPORTS:" << std::endl;
        std::cout << "1. Registration Management" << std::endl;
        std::cout << "2. View Reports & Statistics" << std::endl;
        
        std::cout << "\nPLAYER OPERATIONS:" << std::endl;
        std::cout << "3. Check-In Management" << std::endl;
        std::cout << "4. Player Entry & Withdrawal" << std::endl;
        
        std::cout << "\nSYSTEM:" << std::endl;
        std::cout << "0. Exit System" << std::endl;
        
        std::cout << "\n====================================================" << std::endl;
        std::cout << "Data auto-saves to: players.csv after each change" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "Enter your choice: ";
    }
    
    // Registration Management submenu
    void displayRegistrationMenu() {
        clearScreen();
        std::cout << "\n============= REGISTRATION MANAGEMENT =============" << std::endl;
        std::cout << "1. Display Registration Queue" << std::endl;
        std::cout << "2. Display Players by Priority" << std::endl;
        std::cout << "3. Display Players by Status" << std::endl;
        std::cout << "0. Back to Main Menu" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "Enter your choice: ";
    }
    
    // Reports & Statistics submenu
    void displayReportsMenu() {
        clearScreen();
        std::cout << "\n============= REPORTS & STATISTICS ================" << std::endl;
        std::cout << "1. Generate Registration Report" << std::endl;
        std::cout << "2. Display Queue Summary" << std::endl;
        std::cout << "3. Export Checked-in Players" << std::endl;
        std::cout << "0. Back to Main Menu" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "Enter your choice: ";
    }
    
    // Check-In Management submenu
    void displayCheckInMenu() {
        clearScreen();
        std::cout << "\n============== CHECK-IN MANAGEMENT ================" << std::endl;
        std::cout << "1. Check In Individual Player" << std::endl;
        std::cout << "2. View Check-In Status" << std::endl;
        std::cout << "0. Back to Main Menu" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "Enter your choice: ";
    }
    
    // Player Entry & Withdrawal submenu
    void displayPlayerOpsMenu() {
        clearScreen();
        std::cout << "\n=========== PLAYER ENTRY & WITHDRAWAL =============" << std::endl;
        std::cout << "1. Add Wildcard Entry" << std::endl;
        std::cout << "2. Handle Player Withdrawal" << std::endl;
        std::cout << "3. View Player Details" << std::endl;
        std::cout << "0. Back to Main Menu" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "Enter your choice: ";
    }
    
    // Clear entire queue
    void clearQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }
    
    // Find player by ID
    Player findPlayerByID(const std::string& playerID) {
        PriorityQueueNode* current = front;
        while (current != nullptr) {
            if (current->player.playerID == playerID) {
                return current->player;
            }
            current = current->next;
        }
        return Player(); // Return empty player if not found
    }
    // View player details
    void viewPlayerDetails() {
        if (isEmpty()) {
            std::cout << "Registration queue is empty!" << std::endl;
            return;
        }
        
        std::string playerID;
        std::cout << "Enter Player ID to view details: ";
        std::cin >> playerID;
        
        PriorityQueueNode* current = front;
        while (current != nullptr) {
            if (current->player.playerID == playerID) {
                std::cout << "\n========== PLAYER DETAILS ==========" << std::endl;
                std::cout << "Player ID: " << current->player.playerID << std::endl;
                std::cout << "Name: " << current->player.playerName << std::endl;
                std::cout << "University: " << current->player.university << std::endl;
                std::cout << "Game Specialty: " << current->player.gameSpecialty << std::endl;
                std::cout << "Skill Rating: " << current->player.skillRating << std::endl;
                std::cout << "Registration Type: " << current->player.registrationType << std::endl;
                std::cout << "Priority: " << current->player.priority << std::endl;
                std::cout << "Status: " << current->player.status << std::endl;
                std::cout << "====================================" << std::endl;
                return;
            }
            current = current->next;
        }
        
        std::cout << "Player with ID '" << playerID << "' not found." << std::endl;
    }
};

// Main function with organized menu system
int main() {
    TournamentRegistrationQueue registrationSystem;
    int choice, subChoice;
    
    std::cout << "=========================================" << std::endl;
    std::cout << "  APUEC TOURNAMENT REGISTRATION SYSTEM  " << std::endl;
    std::cout << "  Task 2: Player Queueing & Check-ins   " << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // Auto-load player data on startup
    std::cout << "\nAuto-loading player data..." << std::endl;
    registrationSystem.loadPlayersFromCSV("C:\\APU\\GitHub\\C++\\APU-University-eSports-Championship-Management-System\\source\\data\\players.csv");
    std::cout << "\nSystem ready! Players loaded and prioritized." << std::endl;
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
    
    do {
        registrationSystem.displayMainMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1: { // Registration Management
                do {
                    registrationSystem.displayRegistrationMenu();
                    std::cin >> subChoice;
                    
                    switch (subChoice) {
                        case 1:
                            registrationSystem.clearScreen();
                            registrationSystem.displayRegistrationQueue();
                            break;
                        case 2:
                            registrationSystem.clearScreen();
                            registrationSystem.displayPlayersByPriority();
                            break;
                        case 3:
                            registrationSystem.clearScreen();
                            registrationSystem.displayPlayersByStatus();
                            break;
                        case 0:
                            break;
                        default:
                            std::cout << "Invalid choice! Please try again." << std::endl;
                    }
                    
                    if (subChoice != 0) {
                        std::cout << "\nPress Enter to continue...";
                        std::cin.ignore();
                        std::cin.get();
                    }
                } while (subChoice != 0);
                break;
            }
            
            case 2: { // Reports & Statistics
                do {
                    registrationSystem.displayReportsMenu();
                    std::cin >> subChoice;
                    
                    switch (subChoice) {
                        case 1:
                            registrationSystem.clearScreen();
                            registrationSystem.generateRegistrationReport();
                            break;
                        case 2:
                            registrationSystem.clearScreen();
                            registrationSystem.displayQueueSummary();
                            break;
                        case 3: {
                            registrationSystem.clearScreen();
                            std::string filename;
                            std::cout << "Enter filename to export checked-in players: ";
                            std::cin >> filename;
                            registrationSystem.exportCheckedInPlayersForMatching(filename);
                            break;
                        }
                        case 0:
                            break;
                        default:
                            std::cout << "Invalid choice! Please try again." << std::endl;
                    }
                    
                    if (subChoice != 0) {
                        std::cout << "\nPress Enter to continue...";
                        std::cin.ignore();
                        std::cin.get();
                    }
                } while (subChoice != 0);
                break;
            }
            
            case 3: { // Check-In Management
                do {
                    registrationSystem.displayCheckInMenu();
                    std::cin >> subChoice;
                    
                    switch (subChoice) {
                        case 1:
                            registrationSystem.clearScreen();
                            registrationSystem.checkInPlayer();
                            break;
                        case 2:
                            registrationSystem.clearScreen();
                            registrationSystem.viewCheckInStatus();
                            break;
                        case 0:
                            break;
                        default:
                            std::cout << "Invalid choice! Please try again." << std::endl;
                    }
                    
                    if (subChoice != 0) {
                        std::cout << "\nPress Enter to continue...";
                        std::cin.ignore();
                        std::cin.get();
                    }
                } while (subChoice != 0);
                break;
            }
            
            case 4: { // Player Entry & Withdrawal
                do {
                    registrationSystem.displayPlayerOpsMenu();
                    std::cin >> subChoice;
                    
                    switch (subChoice) {
                        case 1:
                            registrationSystem.clearScreen();
                            registrationSystem.addWildcardEntry();
                            break;
                        case 2: {
                            registrationSystem.clearScreen();
                            std::string playerID;
                            std::cout << "Enter Player ID to withdraw: ";
                            std::cin >> playerID;
                            registrationSystem.handleWithdrawal(playerID);
                            break;
                        }
                        case 3:
                            registrationSystem.clearScreen();
                            registrationSystem.viewPlayerDetails();
                            break;
                        case 0:
                            break;
                        default:
                            std::cout << "Invalid choice! Please try again." << std::endl;
                    }
                    
                    if (subChoice != 0) {
                        std::cout << "\nPress Enter to continue...";
                        std::cin.ignore();
                        std::cin.get();
                    }
                } while (subChoice != 0);
                break;
            }
            
            case 0: {
                registrationSystem.clearScreen();
                std::cout << "Exiting Tournament Registration System..." << std::endl;
                std::cout << "All data has been automatically saved. Goodbye!" << std::endl;
                break;
            }
            
            default: {
                std::cout << "Invalid choice! Please try again." << std::endl;
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
            }
        }
        
    } while (choice != 0);
    
    return 0;
}