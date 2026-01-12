# TODO
- Qx Table code를 만들었어. 이건 data_helper에 있어.
- 이제 insurance_output_data_Structure.cc에서 이미 Dnum은 있으니 이것을 통해 실제 어떤 Qx Table이 필요한지 이름을 추출하고 그 이름을 통해 Qx table을 가져와서 그 다음을 진행해야돼.

## Overview
- Qx Calculation부분을 해야한다.

## Files
- `actuarial_calculator.h` - 보험수리 계산 함수 인터페이스
- `actuarial_calculator.cc` - 보험수리 계산 함수 구현
- `insurance_output_data_structure.cc` - 현재 계산을 구현하고 있는 부분(마지막 부분에 해야한다.)

## Functions

### 1. QxDistribution(actuarial_calculator)
- Qx distribution을 하는 함수이다..
- QxDataStructure에서 값을 가져와서 새로운 자료구조에 넣어줘야 한다.
- 새로운 자료구조는 insurance_output_data_Structure에다가 구현해야 한다.

## TODO
- 현재 copilot이 만들어준 argument는 qx_in이 함수인데 나는 하나의 자료구조를 만들어서 전해줘야 한다.
- 이것을 위해서는 qx_in이 뭔지 알아야 한다. 그걸 알기 위해서는 내가 지금 parsing한 qx 자료구조를 어떻게 구성해서 qx_in을 만드는지 알아야 한다.