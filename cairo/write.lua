template = {}
template.cost = 100

stream = io.open("tmp/cust.txt", "w")
stream:write(template.cost, "\n")
