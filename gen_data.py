import csv
from faker import Faker
import random

fake = Faker()

NUM_ROWS = 10000

with open('data.csv', 'w', newline='') as csvfile:
    csv_writer = csv.writer(csvfile, lineterminator="\n")

    csv_writer.writerow(['Id', 'Username', 'Connected Names'])

    id = 1
    for _ in range(NUM_ROWS):
        username = fake.user_name()

        num_connected_names = random.randint(1,  7)
        connected_names = [fake.user_name() for _ in range(num_connected_names)]

        csv_writer.writerow([id, username, *connected_names])
        id += 1
