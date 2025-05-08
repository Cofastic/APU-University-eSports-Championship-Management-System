# Task 1: Match Scheduling & Player Progression

## 📋 Task Description

This module is responsible for organizing the competition's structure and managing player advancement through various tournament stages for the Asia Pacific University Esports Championship (APUEC).

## 🎯 Key Responsibilities

- **Ensure fair match pairings** based on player rankings
- **Manage the flow of matches** throughout the tournament
- **Update tournament brackets dynamically** as players progress

## 🏗️ Data Structures Used

This implementation will utilize custom implementations of:
- **Stacks**
- **Queues**
- **Priority Queues**

> Note: As per assignment requirements, STL containers like `<list>` or `<vector>` will not be used.

## 💾 Datasets

The system will work with the following CSV files:

### players.csv
```csv
player_id,name,university,game_id,email,phone,rank,registration_time,is_early_bird,is_wildcard,check_in_status,registration_status
P001,John Doe,Asia Pacific University,JohnD#1234,john@example.com,+60123456789,1254,2025-04-01 09:15:32,true,false,checked_in,confirmed
P002,Jane Smith,University of Tokyo,JaneS#5678,jane@example.com,+81987654321,982,2025-04-02 14:30:45,true,false,not_checked,confirmed
```

### registration_events.csv
```csv
event_id,timestamp,event_type,player_id,details
E001,2025-04-15 09:00:00,check_in,P001,Player checked in at registration desk
E002,2025-04-15 09:05:30,check_in,P004,Player checked in at registration desk
```

## 🔄 Data Structure Implementation Details

### 1. Match Scheduling

A **Queue-based system** will be implemented to:
- Maintain the sequence of upcoming matches
- Process matches in a fair order (first come, first served)
- Handle player check-ins efficiently

### 2. Tournament Progression

A **Stack-based approach** will track:
- Player advancement through tournament stages
- History of player progression
- Support for tournament administration tasks


### 3. Player Ranking and Seeding

A **Priority Queue** will be used to:
- Organize players based on rankings
- Create fair match pairings
- Prioritize certain matches in the schedule

## 🔄 Integration with Other Tasks

This task will:
- Receive player data from **Task 2: Tournament Registration**
- Provide match results to **Task 4: Game Result Logging**
- Share tournament schedule information with **Task 3: Live Stream Management**
