import os
import matplotlib.pyplot as plt
import numpy as np

def get_o_file_sizes(directory):
    """Retrieve the sizes of all .o files in the directory in kilobytes."""
    file_sizes = {}
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".o"):
                file_path = os.path.join(root, file)
                file_size_kb = os.path.getsize(file_path) / 1024  # Convert to KB
                file_sizes[file] = file_size_kb
    return file_sizes

def group_file_sizes(file_sizes, groups):
    """Group file sizes based on user-defined groups."""
    grouped_sizes = {group_name: 0 for group_name in groups.keys()}

    # Calculate sizes for each group
    for file, size in file_sizes.items():
        for group_name, patterns in groups.items():
            if any(pattern in file for pattern in patterns):
                grouped_sizes[group_name] += size
                break

    return grouped_sizes


import matplotlib.pyplot as plt
import numpy as np

def plot_group_totals(grouped_sizes, total_size):
    """Plot the bar plot with group totals and an overall total line."""
    fig, ax = plt.subplots()

    # Bar plot for group totals only
    group_names = list(grouped_sizes.keys())
    group_totals = list(grouped_sizes.values())
    bar_positions = np.arange(len(group_names))
    
    bars = ax.bar(bar_positions, group_totals, label='Group Totals', color='b', alpha=0.6)

    # Adding group names under each bar
    ax.set_xticks(bar_positions)
    ax.set_xticklabels(group_names, rotation=45, ha='center')

    # Display the group totals above each bar
    for i, total in enumerate(group_totals):
        ax.text(i, total + 1, f'{total:.2f} KB', ha='center', va='bottom', color='red')

    margin = 1.5  # Margin above the total line
    ax.axhline(y=total_size + margin, color='r', linestyle='--', label='Overall Total Size')

    # Adding a text label for the overall total size
    ax.text(len(group_names) - 1, total_size + 1, f'Total: {total_size:.2f} KB', 
            ha='right', va='bottom', color='r')

    ax.set_ylabel('Group Total Size (KB)', color='b')
    ax.legend()

    plt.title("Total in memory sizes")
    plt.tight_layout()
    plt.show()



if __name__ == "__main__":
    directory = "../code/"  # Set this to your directory containing .o files
    groups = {

        'SM state management': ['delete', 'getters'],   # Replace 'file1', 'file2' with patterns of files for Group 1
        'PMP management': ['pmp','boot','set_pmp'],   # Replace 'file3', 'file4' with patterns of files for Group 2
        'Security Manager':['sm','lib','memory'],
        'Untrusted library':['untrusted_boot','untrusted_sm_calls','untrusted_lilb'],
        'HACL*':['hacl-hmac','shared_mem_lib']
    }

    file_sizes = get_o_file_sizes(directory)  # Retrieve sizes of .o files
    grouped_sizes = group_file_sizes(file_sizes, groups)  # Calculate group totals
    print(grouped_sizes)
    grouped_sizes['Untrusted library'] = 8.7;
    total_size = 325.82# Total size of all .o files

    # Now call the plotting function to display group totals
    plot_group_totals(grouped_sizes, total_size)
