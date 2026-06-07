# TODO
## SRatio_Calc의 PostProcess
- 이제 Benefit함수들 수정해야 한다.
    - 그냥 다 지우고 처음부터 만들자.
- 현재 Pv함수를 수정하고 있다.
    - Pv함수는 예전에 만들어놓은 함수로 argument로 너무 많은것들을 받고 있다.(완료)
    - 현재 이 argument들은 전부 기존의 자료구조에 저장되어있는 것들로 적절하게 struct들을 전달해준다면 함수를 깔끔하게 수정할 수 있다.(완료)
- 그 뒤에 JHJ_flag부터 진행해야 한다.(완료)
    - 현재 Copilot한테 질문해놓은게 있다.(lx등 자료구조를 어떻게 다뤄야 할까?)(완료)
    - lx, Dx, Nx, C0x, M0x는 영구 저장 데이터가 아닙니다. VBA에서 전역인 이유는 전역이 VBA의 유일한 "함수 간 공유" 수단이기 때문이고, 실제로는 Computation → MxStep → PV 간의 임시 패스스루 버퍼입니다.(완료)
- S_Ratio_Calc에 있는 Qx_Distribution부분 구현중이다.(완료)
    - 현재 내가 하고있는 것은 C++코드와 VBA코드의 QxDistribution결과를 비교하는 것이다.(완료)