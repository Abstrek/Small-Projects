import requests

#URL for the api and corresponding api key for retrieving the JSON file with ExchangeRates
base_url = "https://v6.exchangerate-api.com/v6/"
api_key = "cbf9a01d9f3ecf79d7844f5e"

#Seperate function for these two since exception handling easier this way
def enter_amount_currency():
    #Basic exception handling for inputing the value again and again until value of right datatype is enter
    try:
        num = int(input("Enter number of currencies you want the amount to be converted into: "))
    except ValueError:
        print("'ERROR': Please enter a 'numerical value' enter the values again!!!")
        num = int(input("Enter number of currencies you want the amount to be converted into: "))
    return num

#Same thing here but for the amount
def enter_amount():
    try:
        amount = float(input("Amount that needs to be converted: "))
    except ValueError:
        print("'ERROR': Please enter a 'numerical value' enter the values again!!!")
        amount = input("Amount that needs to be converted: ")
    return amount

#The main function
def main():

    #The main three inputs
    amount = enter_amount()

    currency_symbol = input("Enter the symbol for the request currency: ").upper()

    num = enter_amount_currency()

    for i in range(0, num):
        conversion_symbol = input(f"Enter the symbol for the currency {i} to be converted into: ").upper()
        request_url = f"{base_url}{api_key}/latest/{currency_symbol}"
        print(request_url)
        response = requests.get(request_url)

        if response.status_code == 200:
            data = response.json()
            conversion = float(data['conversion_rates'][conversion_symbol])

            print(f"The current conversion rate from {currency_symbol} to {conversion_symbol} is {conversion}")

            converted_amount = round(amount * conversion, 2)
            print(f"The amount after conversion is {converted_amount} {conversion_symbol}")

        else:
            print(f"Error: {response.status_code}")

    choice = input("Enter 'Y' to retry or any other character to shutdown: ").upper()
    if choice == "Y":
        main()

#calling the main function in the end
main()
