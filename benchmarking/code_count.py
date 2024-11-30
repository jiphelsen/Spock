import os

# Object to store the statistics for plotting later
class LineCountStats:
    def __init__(self, group_name):
        self.group_name = group_name
        self.lines_per_language = {
            'C': 0,
            'Header': 0,
            'Assembly': 0
        }
        self.total_lines = 0

    def add_language_lines(self, language, count):
        self.lines_per_language[language] += count

    def calculate_total(self):
        self.total_lines = sum(self.lines_per_language.values())

# Function to count lines in a group of files and return the stats object
def count_lines_in_group(group_name, file_names, directory):
    stats = LineCountStats(group_name)

    # File extensions mapped to languages
    extensions_mapping = {
        '.c': 'C',
        '.h': 'Header',
        '.s': 'Assembly'
    }

    # Count lines for each file in the group
    for file in file_names:
        ext = os.path.splitext(file)[1]

        # Ignore .o files
        if ext == '.o':
            continue

        # Check if the file has an extension we care about
        if ext in extensions_mapping:
            language = extensions_mapping[ext]

            # Count lines in the file
            file_path = os.path.join(directory, file)
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                lines = f.readlines()
                non_empty_lines = [line for line in lines if line.strip()]
                stats.add_language_lines(language, len(non_empty_lines))

    # Calculate total for the group
    stats.calculate_total()

    # Print the result per language for the group
    print(f"Group: {group_name}")
    for lang, count in stats.lines_per_language.items():
        print(f"  {lang}: {count} lines")
    print(f"  Total: {stats.total_lines} lines\n")

    return stats

# Function to collect files based on patterns
def collect_files_by_pattern(directory, patterns):
    matching_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if any(pattern in file for pattern in patterns):
                matching_files.append(file)
    return matching_files

# Function to count lines in all files and return the stats object
def count_lines_in_all_files(directory,substract):
    stats = LineCountStats('All Files')

    
    extensions_mapping = {
        '.c': 'C',
        '.h': 'Header',
        '.s': 'Assembly'
    }


    # Walk through all files in the directory
    for root, _, files in os.walk(directory):
        for file in files:
            print(file)
            ext = os.path.splitext(file)[1]

            # Ignore .o files
            if ext == '.o':
                continue

            # Check if the file has an extension we care about
            if ext in extensions_mapping:
                language = extensions_mapping[ext]

                # Count lines in the file
                file_path = os.path.join(root, file)
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    lines = f.readlines()
                    non_empty_lines = [line for line in lines if line.strip()]
                    stats.add_language_lines(language, len(non_empty_lines))

    # Calculate total for all files
    stats.calculate_total()

    # Print total per language
    print(f"Total Lines for All Files:")
    for lang, count in stats.lines_per_language.items():
        print(f"  {lang}: {count} lines")
    print(f"  Grand Total: {stats.total_lines} lines\n")

    return stats

# Example usage
base_directory = '../code/'

# Group 1: Enclave-related files
group1_patterns = ['enclave1','enclave2', 'hacl']
group1_files = collect_files_by_pattern(base_directory, group1_patterns)
group1_stats = count_lines_in_group('untrusted', group1_files, base_directory)

# Group 2: Delete and Memory-related files
group2_patterns = ['delete', 'memory','getters']
group2_files = collect_files_by_pattern(base_directory, group2_patterns)
group2_stats = count_lines_in_group('Delete and Memory-related', group2_files, base_directory)

# Group 3: PMP, Boot, and set_pmp files
group3_patterns = ['pmp', 'boot', 'set_pmp']
group3_files = collect_files_by_pattern(base_directory, group3_patterns)
group3_stats = count_lines_in_group('PMP, Boot, and set_pmp', group3_files, base_directory)


# Group 3: PMP, Boot, and set_pmp files
group4_patterns = ['untrusted_lib', 'untrusted_boot', 'untrusted_sm_calls',"shared_lib"]
group4_files = collect_files_by_pattern(base_directory, group4_patterns)
group4_stats = count_lines_in_group('Support group', group4_files, base_directory)


# Count lines for all files in the project
all_files_stats = count_lines_in_all_files(base_directory,group1_stats)

import numpy as np
import matplotlib.pyplot as plt

def plot_line_count_stats_combined(groups, group_names):
    # Check if the provided objects are instances of LineCountStats
    for group in groups:
        if not isinstance(group, LineCountStats):
            raise TypeError(f"Expected 'LineCountStats' object, but got {type(group).__name__}")

    # Prepare to sum C and Header
    combined_counts = []
    for group in groups:
        combined_count = group.lines_per_language['C'] + group.lines_per_language['Header']
        combined_counts.append(combined_count)

    # Prepare Assembly counts
    assembly_counts = [group.lines_per_language['Assembly'] for group in groups]

    bar_width = 0.35
    index = np.arange(len(groups))

    fig, ax = plt.subplots(figsize=(10, 6))

    # Plot combined C and Header as one bar and Assembly as another
    ax.bar(index, combined_counts, bar_width, label='C + Header')
    ax.bar(index + bar_width, assembly_counts, bar_width, label='Assembly')

    # Plot total lines as a separate line plot
    totals = [group.total_lines for group in groups]
    ax.plot(index + bar_width / 2, totals, marker='o', color='black', label='Total Lines', linestyle='--')

    ax.set_ylabel('Lines of Code')
    ax.set_xticks(index + bar_width / 2)
    ax.set_xticklabels(group_names)
    
    ax.legend()
    plt.tight_layout()
    plt.show()

# Create an instance for Security Manager
security_manager = LineCountStats("Security Manager")

# Calculate statistics for Security Manager by subtracting the total lines
for language in ['C', 'Header']:
    security_manager.lines_per_language[language] = (
        all_files_stats.lines_per_language[language] -
        (group2_stats.lines_per_language[language] +
         group3_stats.lines_per_language[language] +
         group4_stats.lines_per_language[language])
    )

# Calculate the total lines for Security Manager
security_manager.calculate_total()

# Create an instance for Trusted Computing Base
trusted_computing_base = LineCountStats("TCB")

# Calculate statistics for Trusted Computing Base by subtracting lines from group3_patterns
for language in ['C', 'Header', 'Assembly']:
    trusted_computing_base.lines_per_language[language] = (
        all_files_stats.lines_per_language[language] - 
        group3_stats.lines_per_language[language]
    )

# Calculate the total lines for the Trusted Computing Base group
trusted_computing_base.calculate_total()


group_names = ["SM state management", "PMP management", "Untrusted library","Security Manager","TCB" ,"Total"]
plot_line_count_stats_combined([group2_stats, group3_stats, group4_stats,security_manager,trusted_computing_base, all_files_stats], group_names)