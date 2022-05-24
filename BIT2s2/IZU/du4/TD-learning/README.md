# TD learning algorithm

This is simple python script for calculating 
new utility values in a single step of the TD algorithm.

## Usage

First fill:
- Actual state of the utility values
- Requested path
- Alfa, gamma coefficients

Then run:

```
python TD-learn.py 
```

## Examples 

Example from IZU materials (page: 136)

Initial state:
```
Alpha = 0.1
Gamma = 0.9
Path = "1 2 3 6 9"
Utilities = \
"""
  0.000  0.000  0.000
  0.000  0.000  0.000
  0.000 rew=-1  rew=1
""" 
```

I. path:
```
  0.000  0.000  0.000
  0.000  0.000  0.100
  0.000 rew=-1  rew=1
```

II. path (1 2 5 2 3 6 9):
```
  0.000  0.000  0.009
  0.000  0.000  0.190
  0.000 rew=-1  rew=1`
```

III. path (1 4 1 2 3 6 9)
```
  0.000  0.001  0.025
  0.000  0.000  0.271
  0.000 rew=-1  rew=1
```

IV. path (1 4 7 8)
```
  0.000  0.001  0.025
  0.000  0.000  0.271
 -0.100 rew=-1  rew=1
```

V. path (1 2 5 8)
```
  0.000  0.001  0.025
  0.000 -0.100  0.271
 -0.100 rew=-1  rew=1
```

