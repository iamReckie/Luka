# 현재 하는것
- Excel file의 vba코드에 보면 varTemp1과 varTemp2별로 실제로 construct하는 자료구조가 다름
- **calc_reserve_expense.cc** 파일을 보면 어느 자료구조를 구성할지에 대해 if else if로 처리되어 있음.
# 문제
- 현재 data_helper로 args를 보내는데 어떤 자료구조를 구성할지에 대한 정보를 보내지 않음
- 내가 원하는 방식은 args에 원하는 자료구조, index 원하는 자료구조, index 이런식으로 만들고 싶음.
- args에 key value형식으로 추가를 해서 reserve_result_data.cc에서 상황에 맞게 자료구조를 구성하는 방식으로 다시 생각해야됨.