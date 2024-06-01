import re
from my_set import MySet

def get_file_content(filepath: str) -> str:
    ...
    with open(filepath, 'r') as f:
        return f.read()
    
def deliverable_3() -> None:
    ...
    """
        I will use regular expressions to extract the functions from the Util.h file.
        I will write the results to a file called regex_results.txt.
        (?<=\n|\s|\.)[A-Za-z_]\w*\s*\((?=.*?)\)
        '?<=' - Positive lookbehind assertion checking for '\n' or '\s' or '.'
        then '[A-Za-z_]\w*' - matches the function name
        then '\s*' - matches the space between the function name and the opening parenthesis if any
        then '\(' - matches the opening parenthesis
        then '(?=.*?)' - matches the parameters of the function, not capturing the parameters 
        
        I also used my own set implementation to remove duplicates from the list of functions.
    """
    # ============================ Regular Expression ============================
    code = get_file_content('Util.h')
    
    function_pattern = re.compile(r'(?<=\n|\s|\.)[A-Za-z_]\w*\s*\((?=.*?)\)', flags=re.M)
    functions = function_pattern.findall(code)
    
    functions = MySet(functions)
    
    for function in functions:
        print(function)
    
    with open('regex_results.txt', 'w') as f:
        f.write("============================ RegEx Results ============================\n")
        f.write("Functions:\n")
        f.write(str(functions))
    

def main() -> None:
    ...
    deliverable_3()
        

if __name__ == '__main__':
    main()