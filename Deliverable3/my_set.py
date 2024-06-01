from sys import argv

class MySet:
    def __init__(self, elements: list = None) -> None:
        ...
        self.elements = []
        if elements is not None:
            for element in elements:
                self.add(element)
        
    def add(self, element: any) -> None:
        ...
        if element not in self.elements:
            self.elements.append(element)
            
    def remove(self, element: any) -> None:
        ...
        if element in self.elements:
            self.elements.remove(element)
    
    def __str__(self) -> str:
        ...
        return str(self.elements)
    
    def __len__(self) -> int:
        ...
        return len(self.elements)
    
    def __contains__(self, element: any) -> bool:
        ...
        return element in self.elements
    
    def __eq__(self, other: 'MySet') -> bool:
        ...
        return self.elements == other.elements
    
    def __iter__(self):
        ...
        return iter(self.elements)


def main() -> None:
    ...
    my_set = MySet([1, 2, 3])
    my_set.add(4)
    my_set.add(4)
    my_set.add(5)
    my_set.add('yes')
    print(my_set)

if __name__ == '__main__':
    main()