config = {
  verbose: true,
  debug: false,
  broadcast: true,
  forceActionDataHex: true,
  sign: true,
  keyProvider: keyProvider // Defined in config.js
};

eos = Eos(config)

// NOTE: account is defined in config.js

function newgame() {
  eos.transaction({
    actions: [{
      account: account,
      name: 'newgame',
      authorization: [{
        actor: account,
        permission: 'active'
      }],
      data: {
        account: account,
      }
    }]
  });
}

function endgame() {
  eos.transaction({
    actions: [{
      account: account,
      name: 'endgame',
      authorization: [{
        actor: account,
        permission: 'active'
      }],
      data: {
        account: account,
      }
    }]
  });
}

function play(row, col) {
  eos.transaction({
    actions: [{
      account: account,
      name: 'play',
      authorization: [{
        actor: account,
        permission: 'active'
      }],
      data: {
        account: account,
        row: row,
        col: col
      }
    }]
  });
}
