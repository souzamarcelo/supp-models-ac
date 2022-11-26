string determineTenure() {
    string value;
    
    switch (pTenureType.back()) {

        case 'a':
          preamble += "double davg = double(accumulate(I.deg.begin(),I.deg.end(),0))/I.n;";
          value = "max(1u, unsigned(davg+1))";
          break;

        case 'v':
          value = "I.n";
          break;

        case 'p': {
          string t = to_string(pTenurePercent);
          t.pop_back();
          value = "max(1.0," + t + " * I.n / 100.0)";
          break;
        }

        case 'c': {
          string t = to_string(pTenureDivision);
          t.pop_back();
          value = "I.n / " + t;
          break;
        }

        case 'r': {
          value = "(I.n / " + to_string(pCm) + ") + uniform_int_distribution<>(1, " + to_string(pTenureVariation) + ")(rng)";
          break;
        }

        case 'n':
          value = "max(1, " + to_string(pTenureValue) + ")";
          break;

        default:
          value = "<undefined>";
          break;
    }

    return value;
}

string determineMaxStagnate() {
  string value;

  switch (pMaxStagnateType.back()) {

    case 'a':
      value = to_string(pIc) + " * I.n";
      break;

    case 'i':
      value = "numeric_limits<unsigned>::max()";
      break;

    case 'm':
      value = to_string(pMum) + " * I.n";
      break;

    case 'n':
      value = "max(1," + to_string(pMaxStagnateValue) + ")";
      break;

    default:
      value = "<undefined>";
      break;
  }

  return value;
}

string determineMaxSteps() {
  string value;

  switch (pMaxStepsType.back()) {

    case 'a':
      value = "I.n>5000?15000:(I.n>3000?12000:10000)";
      break;

    case 'i':
      value = "numeric_limits<unsigned>::max()";
      break;

    case 'n':
      value = "max(1," + to_string(pMaxStepsValue) + ")";
      break;

    default:
      value = "<undefined>";
      break;
  }

  return value;
}