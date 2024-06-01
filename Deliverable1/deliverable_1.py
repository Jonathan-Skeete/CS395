import requests
from bs4 import BeautifulSoup
import re
import pandas as pd
import matplotlib.pyplot as plt

def get_file_content(filepath: str) -> str:
    ...
    with open(filepath, 'r') as f:
        return f.read()
    
def first_plot(df: pd.DataFrame) -> None:
    ...
    plt.style.use('fivethirtyeight')
    plt.rcParams['font.size'] = 10
    
    fig, ax = plt.subplots(figsize=(10, 6))
    df.plot(kind='barh', x='President', y='Salary', ax=ax)
    plt.title('2015 Compensation of Private Ohio College Presidents')
    
    ax.set_xlabel('Total Compensation ($)')
    ax.set_ylabel('President')
    
    for index, row in df.iterrows():
        ax.text(row['Salary'] * 0.5, index, row['College'], va='center', ha='center', fontsize=8, color='white')
        ax.text(row['Salary'] + 5000, index, f"${row['Salary']:,}", va='center', fontsize=10, color='black')
    
    plt.tight_layout()

    plt.savefig('visualization1.png')
    
    
def second_plot(df: pd.DataFrame) -> None:
    ...
    college_salaries = df.groupby('College')['Salary'].sum().sort_values(ascending=True)
    plt.style.use('fivethirtyeight')
    plt.rcParams['font.size'] = 10
    
    fig, ax = plt.subplots(figsize=(10, 6))
    college_salaries.plot(kind='barh', ax=ax)
    plt.title("Value of Five Minutes of Your President's Time")
    
    ax.set_xlabel('Value ($)')
    ax.set_ylabel('College/University')
    
    for index, row  in college_salaries.reset_index().iterrows():
        ax.text(row['Salary'] + 5000, index, f"${round(row['Salary']/24e3):,}", va='center', fontsize=10, color='black')
        
    
    plt.tight_layout()
    plt.savefig('visualization2.png')
    
    
def deliverable_1() -> None:
    ...
    # ============================ Web Scraping ============================
    
    r = requests.get('http://www.cleveland.com/metro/index.ssf/2017/12/case_western_reserve_university_president_barbara_snyders_base_salary_and_bonus_pay_tops_among_private_colleges_in_ohio.html')
    c = r.content
    soup = BeautifulSoup(c, 'html.parser')
    with open('project.html', 'w') as f:
        f.write(soup.prettify())
    
    main_content = soup.find('div', attrs={'class': 'entry-content'}) 
    #print(main_content)   
    content = main_content.find('ul').get_text('\n')
    # print(content, end='\n\n')
    with open('web_scraping_results.txt', 'w') as f:
        f.write("============================ Web Scraping Results ============================\n")
        f.write(content)
    
    # ============================ Regular Expression ============================
    name_pattern = re.compile(r'^([A-Z]{1}.+?)(?:,)', flags=re.M)
    names = name_pattern.findall(content)
    # print(names, end='\n\n')
    
    school_pattern = re.compile(r'(?:,|,\s)([A-Z]{1}.*?)(?:\s\(|:|,)')
    schools = school_pattern.findall(content)
    # print(schools, end='\n\n')
    
    salary_pattern = re.compile(r'\$.+')
    salaries = salary_pattern.findall(content)
    # print(salaries, end='\n\n')
    
    salary_ints = [int(''.join(s[1:].split(','))) for s in salaries]
    # print(salary_ints, end='\n\n')
    
    with open('regex_results.txt', 'w') as f:
        f.write("============================ RegEx Results ============================\n")
        f.write("Names:\n")
        f.write(str(names))
        f.write("\n\nSchools:\n")
        f.write(str(schools))
        f.write("\n\nSalaries:\n")
        f.write(str(salaries))
        f.write("\n\n")
        f.write(str(salary_ints))
    
    # ============================ Data Visualization ============================
    df = pd.DataFrame({'College': schools, 'President':names, 'Salary': salary_ints})
    df = df.sort_values('Salary', ascending=True).reset_index()
    # print("Data Frame:")
    # print(df, end='\n\n')
    
    with open('Visualization.txt', 'w') as f:
        f.write("============================ Visualization ============================\n")
        f.write(str(df))
        
    first_plot(df)
    second_plot(df)
    plt.show()

def main() -> None:
    ...
    deliverable_1()
    print(get_file_content('web_scraping_results.txt'), end='\n\n')  
    print(get_file_content('regex_results.txt'), end='\n\n')
    print(get_file_content('Visualization.txt'), end='\n\n')
        

if __name__ == '__main__':
    main()