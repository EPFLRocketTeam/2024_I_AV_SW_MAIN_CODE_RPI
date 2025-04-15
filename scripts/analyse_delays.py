import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Function to find the last two session numbers
def get_last_two_sessions(data_folder):
    try:
        with open(os.path.join(data_folder, "last_session.txt"), "r") as f:
            last_session = int(f.read().strip())
        return [last_session, last_session - 1]
    except Exception as e:
        print(f"Error reading last_session.txt: {e}")
        return None

# Function to get the paths of the first 3 flights for each of the last 2 sessions
def get_flight_paths(data_folder, sessions):
    flight_paths = []
    for session in sessions:
        session_flights = []
        for flight_num in range(1, 4):  # First 3 flights (1, 2, 3)
            file_name = f"{session}_{flight_num}.csv"
            file_path = os.path.join(data_folder, file_name)
            if os.path.exists(file_path):
                session_flights.append((file_path, session, flight_num))
            else:
                print(f"Warning: Flight file {file_name} not found!")
        flight_paths.append(session_flights)
    return flight_paths

# Function to read and analyze flight data
def analyze_flight_data(flight_paths):
    all_data = []
    
    for session_flights in flight_paths:
        for file_path, session, flight_num in session_flights:
            try:
                df = pd.read_csv(file_path)
                if 'time' in df.columns and 'uart_round_trip_time' in df.columns:
                    # Calculate loop duration
                    new_lines = df.shape[0]
                    avg_dt = df.time.iloc[new_lines - 1] / new_lines
                    
                    all_data.append({
                        'file_path': file_path,
                        'session': session,
                        'flight_num': flight_num,
                        'df': df,
                        'round_trip_time': df['uart_round_trip_time'],
                        'time': df['time'],
                        'loop_duration': avg_dt
                    })
                else:
                    print(f"Missing required columns in {file_path}")
            except Exception as e:
                print(f"Error processing {file_path}: {e}")
    
    return all_data

# Function to plot round trip time data
def plot_round_trip_time(all_data):
    if not all_data:
        print("No data to plot!")
        return

    plt.figure(figsize=(15, 10))
    fig, axes = plt.subplots(2, 3, num=1)  # num=1 forces it to Figure 
    fig.suptitle('UART Round Trip Time Analysis', fontsize=16)
    
    row_idx = 0
    for session_idx in range(2):
        session_data = [data for data in all_data if data['session'] == all_data[0]['session'] - session_idx]
        if not session_data:
            continue
            
        for flight_idx, data in enumerate(session_data[:3]):
            ax = axes[row_idx, flight_idx]
            
            # Plot vertical bars for round trip time using actual time values for x-axis
            ax.bar(data['time'], data['round_trip_time'], width=data['time'].iloc[1] - data['time'].iloc[0] if len(data['time']) > 1 else 0.1)
            
            # Set title and labels
            ax.set_title(f"Session {data['session']}, Flight {data['flight_num']}")
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('Round Trip Time')
            
            # Calculate statistics for this flight
            rtt = data['round_trip_time']
            min_rtt = rtt.min()
            max_rtt = rtt.max()
            avg_rtt = rtt.mean()
            std_rtt = rtt.std()
            
            # Add statistics as text in the plot
            stats_text = f"Min: {min_rtt:.2f}\nMax: {max_rtt:.2f}\nAvg: {avg_rtt:.2f}\nStd: {std_rtt:.2f}"
            ax.text(0.02, 0.95, stats_text, transform=ax.transAxes, 
                    verticalalignment='top', bbox={'boxstyle': 'round', 'facecolor': 'white', 'alpha': 0.5})
        
        row_idx += 1
    
    plt.tight_layout(rect=[0, 0, 1, 0.95])
    
    # Calculate combined statistics
    all_rtt = pd.concat([data['round_trip_time'] for data in all_data])
    min_rtt = all_rtt.min()
    max_rtt = all_rtt.max()
    avg_rtt = all_rtt.mean()
    std_rtt = all_rtt.std()
    
    print("\nUART Round Trip Time Statistics:")
    print("-" * 40)
    for data in all_data:
        rtt = data['round_trip_time']
        print(f"Session {data['session']}, Flight {data['flight_num']}:")
        print(f"  Min: {rtt.min():.2f}, Max: {rtt.max():.2f}, Avg: {rtt.mean():.2f}, Std: {rtt.std():.2f}")
    
    print("\nCombined Statistics:")
    print(f"  Min: {min_rtt:.2f}, Max: {max_rtt:.2f}, Avg: {avg_rtt:.2f}, Std: {std_rtt:.2f}")
    
    # plt.show()

# Function to plot loop duration data
def plot_loop_duration(all_data):
    if not all_data:
        print("No data to plot!")
        return

    plt.figure(figsize=(15, 10))
    fig, axes = plt.subplots(2, 3, num=2)
    fig.suptitle('Loop Duration Analysis', fontsize=16)
    
    # Calculate loop durations for each data point (difference between consecutive time values)
    for data in all_data:
        time_series = data['time']
        loop_durations = np.diff(time_series)
        # Add a placeholder for the first point (using the calculated average)
        loop_durations = np.insert(loop_durations, 0, data['loop_duration'])
        data['loop_durations'] = loop_durations
    
    row_idx = 0
    for session_idx in range(2):
        session_data = [data for data in all_data if data['session'] == all_data[0]['session'] - session_idx]
        if not session_data:
            continue
            
        for flight_idx, data in enumerate(session_data[:3]):
            ax = axes[row_idx, flight_idx]
            
            # Plot vertical bars for loop durations using actual time values for x-axis
            ax.bar(data['time'], data['loop_durations'], width=data['time'].iloc[1] - data['time'].iloc[0] if len(data['time']) > 1 else 0.1)
            
            # Set title and labels
            ax.set_title(f"Session {data['session']}, Flight {data['flight_num']}")
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('Loop Duration (s)')
            
            # Calculate statistics for this flight
            ld = data['loop_durations']
            min_ld = ld.min()
            max_ld = ld.max()
            avg_ld = ld.mean()
            std_ld = ld.std()
            
            # Add statistics as text in the plot
            stats_text = f"Min: {min_ld:.6f}\nMax: {max_ld:.6f}\nAvg: {avg_ld:.6f}\nStd: {std_ld:.6f}"
            ax.text(0.02, 0.95, stats_text, transform=ax.transAxes, 
                    verticalalignment='top', bbox={'boxstyle': 'round', 'facecolor': 'white', 'alpha': 0.5})
        
        row_idx += 1
    
    plt.tight_layout(rect=[0, 0, 1, 0.95])
    
    # Calculate combined statistics
    all_ld = np.concatenate([data['loop_durations'] for data in all_data])
    min_ld = all_ld.min()
    max_ld = all_ld.max()
    avg_ld = all_ld.mean()
    std_ld = all_ld.std()
    
    print("\nLoop Duration Statistics:")
    print("-" * 40)
    for data in all_data:
        ld = data['loop_durations']
        print(f"Session {data['session']}, Flight {data['flight_num']}:")
        print(f"  Min: {ld.min():.6f}, Max: {ld.max():.6f}, Avg: {ld.mean():.6f}, Std: {ld.std():.6f}")
    
    print("\nCombined Statistics:")
    print(f"  Min: {min_ld:.6f}, Max: {max_ld:.6f}, Avg: {avg_ld:.6f}, Std: {std_ld:.6f}")
    
    # plt.show()

def main():
    # Define the data folder path - update this to your actual data folder path
    data_folder = "/media/daniel/63FC-DAA7"
    
    # Get the last two session numbers
    sessions = get_last_two_sessions(data_folder)
    if not sessions:
        print("Failed to determine session numbers. Exiting.")
        return
    
    print(f"Analyzing data for sessions: {sessions}")
    
    # Get the flight paths
    flight_paths = get_flight_paths(data_folder, sessions)
    
    # Analyze the flight data
    all_data = analyze_flight_data(flight_paths)
    
    if not all_data:
        print("No data to analyze. Exiting.")
        return
    
    # Plot round trip time data
    plot_round_trip_time(all_data)
    
    # Plot loop duration data
    plot_loop_duration(all_data)

    plt.show()

if __name__ == "__main__":
    main()