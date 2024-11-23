
const crypto = require('crypto');
let data = {}


exports.PERMISSION = {
    ADMIN: 0,
    READ_ONLY: 1,
}

// Session manager v0.1
exports.generate =  function(){
        let id = crypto.randomBytes(16).toString('hex');
        data[id] = {}
        return {
            session: data[id],
            id: id
        };
    }

exports.get = function(id){
    
        if(!id || !data[id]){
            return this.generate();
        }
        return {
            session: data[id],
            id: id
        }
    }
