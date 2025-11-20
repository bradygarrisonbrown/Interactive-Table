import json

'''
[1, 2, 3]
[4, 5, 6]
[7, 8, 9]

-> maps directly to [1, 2, 3, 4, 5, 6, 7, 8, 9] to reduce complexity

'''
kDummyData = [
    [
        1,  # time in seconds
        [1.1, 1.2, 1.3, 2.1, 2.2, 2.3, 3.1, 3.2, 3.3] # heights
    ],
    [
        42,
        [4.4, 4.5, 4.6, 5.4, 5.5, 5.6, 6.4, 6.5, 6.6]
    ]
]

def main():
    # Create the actual JSON string
    # note: we use , and : as separators to avoid having the empty space between values
    compact_json = json.dumps(kDummyData, separators=(',', ':'))
    readable_json = json.dumps(kDummyData)

    # 3. Print the result
    print(f"readable: {readable_json}")
    print(f"compact: {compact_json}")

if __name__ == "__main__":
    main()